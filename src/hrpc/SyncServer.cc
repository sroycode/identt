/**
 * @project identt
 * @file src/hrpc/SyncServer.cc
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *  SyncServer.cc :  Sync Service
 *
 */
#include <future>
#include <chrono>
#include <thread>
#include <functional>
#include <store/StoreTrans.hpp>
#include "../proto/Query.pb.h"
#include "../proto/Store.pb.h"

#include "SyncServer.hpp"
#include "SyncServiceList.hpp"

#include <hrpc/HrpcClient.hpp>
#include <hrpc/RemoteKeeper.hpp>

#define IDENTT_SYNC_FIRST_CHUNK_SIZE 1000
#define IDENTT_SYNC_CHUNK_SIZE 1000

/**
* Constructor : private default Constructor
*
*/

identt::hrpc::SyncServer::SyncServer(std::shared_ptr<::identt::http::io_whatever> io_whatever_, identt::utils::SharedTable::pointer stptr)
	: identt::utils::ServerBase(stptr),io_whatever(io_whatever_),is_init(false)
{
	DLOG(INFO) << "SyncServer Created" << std::endl;
}

identt::hrpc::SyncServer::~SyncServer ()
{
	DLOG(INFO) << "SyncServer Deleted" << std::endl;
}

/**
 * GetSection : section required
 *
 */
const std::string identt::hrpc::SyncServer::GetSection()
{
	return "hrpc";
}

/**
 * GetRequire : parameters required
 *
 */
const identt::utils::ServerBase::ParamsListT identt::hrpc::SyncServer::GetRequire()
{
	return { "host", "port", "thisurl" };
}


/**
* init : initialize rocksdb and others
*
*/
void identt::hrpc::SyncServer::init(identt::utils::ServerBase::ParamsListT params)
{

	try {
		std::string host  = params[0];
		uint64_t port  = std::stoul(params[1]);
		sharedtable->thisurl.Set(params[2]);

		if (sharedtable->is_master.Get()) {
			// set this in async loop
			std::async( std::launch::async, [this] {
				this->MasterLoop();
			});
		} else {
			// set slave loop

			// sync from pointed master till current before start
			this->SyncFirst();

			// set this in async loop
			std::async( std::launch::async, [this] {
				this->SyncFromMaster();
				// if upgraded to master
				this->MasterLoop();
			});
		}

		server = std::make_shared<HttpServerT>(port);
		server->io_whatever = io_whatever;
		server->config.address=host;

		is_init=true;
		DLOG(INFO) << "SyncServer init 1 here" << std::endl;
		{
			IDENTT_SYNCSERVICELIST_SCOPE_HTTP
		}
		DLOG(INFO) << "SyncServer init 2 here" << std::endl;
		server->start();

	} catch (identt::BaseException& e) {
		DLOG(INFO) << "SyncServer init failed: " << e.what() << std::endl;
		std::rethrow_exception(std::current_exception());
	}
	DLOG(INFO) << "SyncServer LOOP Ends" << std::endl;
}

/**
* stop : shutdown
*
*/
void identt::hrpc::SyncServer::stop()
{
	to_stop_sync.Set(true);
	if (is_init) server->stop();
	DLOG(INFO) << "Sync Server Stop Done" << std::endl;
}

/**
* SyncFirst : sync data from master first time
*
*/
void identt::hrpc::SyncServer::SyncFirst()
{
	// only if slave
	if (sharedtable->is_master.Get()) return;

	// check last one is in sync
	if (!this->CompareLog(sharedtable->master.Get()))
		throw identt::InitialException("Master Slave log mismatch");

	DLOG(INFO) << "comparelog ok" ;
	::identt::hrpc::HrpcClient hclient;
	::identt::store::TransListT data;
	::identt::store::StoreTrans storetrans;
	::identt::store::TransactionT trans;
	this->to_stop_sync.Set(false);

	while(!to_stop_sync.Get()) {
		data.Clear();
		trans.Clear();
		DLOG(INFO) << "update at logc: " << sharedtable->logcounter.Get();
		data.set_endpoint( sharedtable->thisurl.Get() );
		data.set_ts( IDENTT_CURRTIME_MS );
		data.set_lastid( sharedtable->logcounter.Get() );
		data.set_limit( IDENTT_SYNC_FIRST_CHUNK_SIZE );
		DLOG(INFO) << "sending: " << data.DebugString();

		bool status = hclient.SendToRemote(sharedtable,sharedtable->master.Get(),::identt::hrpc::R_TRANSLOG,&data,false);
		// throws so no need to check status
		DLOG(INFO) << "got: " << data.DebugString();
		for (auto i=0; i<data.trans_size(); ++i) {
			trans.Swap( data.mutable_trans(i) );
			if (trans.id() != (sharedtable->logcounter.Get()+1)) break; // out of sync
			storetrans.Commit(sharedtable,&trans,false); // commit as slave
		}
		if ( data.lastid() == data.currid() )  break;
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
	// see if the pointed machine is master or refers to a master
	::identt::hrpc::StateT sstate;
	size_t tolcount=0;
	do {
		sstate.Clear();
		sstate.set_hostname( sharedtable->hostname.Get() );
		sstate.set_master( sharedtable->master.Get() );
		sstate.set_logcounter( sharedtable->logcounter.Get() );
		sstate.set_is_master( sharedtable->is_master.Get() );
		sstate.set_ts( IDENTT_CURRTIME_MS );
		sstate.set_thisurl( sharedtable->thisurl.Get() );
		hclient.SendToRemote(sharedtable,sharedtable->master.Get(),::identt::hrpc::R_REGISTER,&sstate);
		if (sstate.is_master()) {
			// SetHosts called manually as master update will not reach since is_ready not set
			::identt::hrpc::RemoteKeeper rkeeper(sharedtable);
			rkeeper.SetHosts(&sstate);
		} else {
			sharedtable->master.Set( sstate.master() );
		}
		if (++tolcount>2)
			throw identt::InitialException("the pointed machine is not 1 hop away from master");
	} while(!sstate.is_master());
	LOG(INFO) << "Sync Complete, master is set to " << sharedtable->master.Get();
}

/**
* SyncFromMaster : sync data from master
*
*/
void identt::hrpc::SyncServer::SyncFromMaster()
{
	// only if slave
	if (sharedtable->is_master.Get()) return;
	this->to_stop_sync.Set(false);

	// check last one is in sync as the earlier sync may have been with a slave
	if (!this->CompareLog(sharedtable->master.Get()))
		throw identt::InitialException("Master Slave log mismatch");

	::identt::hrpc::HrpcClient hclient;
	::identt::store::TransListT data;
	::identt::store::StoreTrans storetrans;
	::identt::store::TransactionT trans;
	size_t failcount=0;
	uint64_t faildetected = 0;
	::identt::hrpc::RemoteKeeper rkeeper(sharedtable);

	while(!to_stop_sync.Get()) {

		//first check if master has sent updates
		if (sharedtable->transactions.GetSize()>0) {
			data.Clear();
			identt::utils::SharedTable::SharedTrans::MapT tmap;
			sharedtable->transactions.Swap(tmap);
			auto logc = sharedtable->logcounter.Get();
			uint64_t localup =0;
			while ( localup < tmap.size() ) {
				auto it = tmap.find(++logc);
				if (it==tmap.end()) break; // out of sequence
				if (!trans.ParseFromString(it->second)) break; // bad data
				storetrans.Commit(sharedtable,&trans,false); // as slave
				++localup;
			}
			if ( data.trans_size()>0) {
				failcount=0;
			}
			tmap.clear();
		}

		DLOG(INFO) << "update local: " << sharedtable->logcounter.Get();
		// now check master
		data.Clear();
		data.set_endpoint( sharedtable->thisurl.Get() );
		data.set_ts( IDENTT_CURRTIME_MS );
		data.set_lastid( sharedtable->logcounter.Get() );
		data.set_limit( IDENTT_SYNC_CHUNK_SIZE );
		DLOG(INFO) << "sending: " << data.DebugString();

		bool status = hclient.SendToRemote(sharedtable,sharedtable->master.Get(),::identt::hrpc::R_TRANSLOG,&data,true);
		if (status) {
			DLOG(INFO) << "got: " << data.DebugString();
			for (auto i=0; i<data.trans_size(); ++i) {
				trans.Swap( data.mutable_trans(i) );
				if (trans.id() != (sharedtable->logcounter.Get()+1)) break; // out of sync
				storetrans.Commit(sharedtable,&trans,false); // as slave
			}
			failcount=0;
		} else {
			++failcount;
			if (failcount==1) {
				faildetected = IDENTT_CURRTIME_MS;
			} else if (failcount==10) {
				rkeeper.HostUpdate();
			} else if (failcount==20) {
				rkeeper.MasterUpdate(faildetected);
				if (sharedtable->is_master.Get()) {
					// master needs to stop loop
					to_stop_sync.Set(true);
					return;
				}
			}
			DLOG(INFO) << "Missed an update at logc: " << sharedtable->logcounter.Get() << " fc " << failcount;
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( (data.trans_size()>0) ? 20 : 100 ) );
	}
}

/**
* CompareLog : check if master is having correct log
*
*/
bool identt::hrpc::SyncServer::CompareLog(std::string address)
{

	// check last one is in sync
	uint64_t old_logc = sharedtable->logcounter.Get();
	if (old_logc ==0) return true; // blank slave

	::identt::hrpc::HrpcClient hclient;
	::identt::store::StoreTrans storetrans;
	::identt::store::TransactionT trans1;
	trans1.set_id( old_logc-1 );
	DLOG(INFO) << trans1.DebugString();
	DLOG(INFO) << "comparelog sending to " << address;
	bool status = hclient.SendToRemote(sharedtable,address,::identt::hrpc::R_READONE,&trans1,true);
	if (!status) throw identt::InitialException("Cannot reach master for verification");
	DLOG(INFO) << trans1.DebugString();
	if (trans1.notfound()) throw identt::InitialException("Master does not have last log, mismatch");

	// get the same locally
	::identt::store::TransactionT trans2;
	trans2.set_id( old_logc-1 );
	storetrans.ReadOne(sharedtable,&trans2);
	if (trans2.notfound())
		throw identt::InitialException("This instance does not have last log, mismatch");
	return (trans1.ts() == trans2.ts());
}

/**
* MasterLoop : master loop
*
*/
void identt::hrpc::SyncServer::MasterLoop()
{
	// only if master
	if (!sharedtable->is_master.Get()) return;
	DLOG(INFO) << "Entered master loop";
}
