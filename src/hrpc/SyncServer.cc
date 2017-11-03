/**
 * @project identt
 * @file src/hrpc/SyncServer.cc
 * @author  S Roychowdhury <sroycode AT gmail DOT com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2017 S Roychowdhury.
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
#include "SyncServer.hpp"
#include <hrpc/HrpcClient.hpp>
#include <chrono>
#include <thread>
#include <functional>
#include <store/StoreTrans.hpp>
#include "../proto/Query.pb.h"
#include "../proto/Store.pb.h"

#define IDENTT_SYNC_FIRST_CHUNK_SIZE 100
#define IDENTT_SYNC_CHUNK_SIZE 100

/**
* Constructor : private default Constructor
*
*/

identt::hrpc::SyncServer::SyncServer(identt::utils::SharedTable::pointer stptr)
	: identt::utils::ServerBase(stptr)
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
	// return { "host", "port", "hrpcurl" };
	return {};
}


/**
* init : initialize rocksdb and others
*
*/
void identt::hrpc::SyncServer::init(identt::utils::ServerBase::ParamsListT params)
{

	try {
		// std::string host = params[0];
		// std::size_t port = std::stoul(params[1]);
		// std::string hrpcurl = params[2];

		if (sharedtable->is_master.Get()) {
			return ;
		} else {
			// set slave loop
			// see if the pointed machine is master or refers to a master
			::identt::query::StateT sstate;
			size_t tolcount=0;
			::identt::hrpc::HrpcClient hclient;
			do {
				hclient.SendToRemote(sharedtable,sharedtable->master.Get(),::identt::hrpc::R_GETINFO,&sstate);
				if (!sstate.is_master())
					sharedtable->master.Set( sstate.master() );
				if (++tolcount>2)
					throw identt::InitialException("the pointed machine is not 1 hop away from master");
			} while(!sstate.is_master());
			LOG(INFO) << "Master is set to " << sharedtable->master.Get();

			// sync from master till current before start
			this->SyncFirst();
			LOG(INFO) << "Master Sync Complete";
			// set this in async loop
			async::spawn([this] {
				this->SyncFromMaster();
			});
		}

	} catch (identt::JdException& e) {
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
	to_stop.Set(true);
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
	this->to_stop.Set(false);

	// check last one is in sync
	if (!this->CompareLog(sharedtable->master.Get()))
		throw identt::InitialException("Master Slave log mismatch");

	::identt::hrpc::HrpcClient hclient;
	::identt::store::TransListT data;
	::identt::store::StoreTrans storetrans;
	::identt::store::TransactionT trans;


	while(!to_stop.Get()) {
		data.Clear();
		trans.Clear();
		DLOG(INFO) << "update at logc: " << sharedtable->logcounter.Get();
		data.set_lastid( sharedtable->logcounter.Get() );
		data.set_limit( IDENTT_SYNC_FIRST_CHUNK_SIZE );
		DLOG(INFO) << "sending: " << data.DebugString();

		bool status = hclient.SendToRemote(sharedtable,sharedtable->master.Get(),::identt::hrpc::R_TRANSLOG,&data,false);
		if (status) {
			DLOG(INFO) << "got: " << data.DebugString();
			for (auto i=0; i<data.trans_size(); ++i) {
				trans.Swap( data.mutable_trans(i) );
				if (trans.id() != (sharedtable->logcounter.Get()+1)) break; // out of sync
				storetrans.Commit(sharedtable,&trans,false); // commit as slave
			}
			if ( data.lastid() == data.currid() )  return;
		}
		if (!status) LOG(INFO) << "Missed an update at logc: " << sharedtable->logcounter.Get();
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}

/**
* SyncFromMaster : sync data from master
*
*/
void identt::hrpc::SyncServer::SyncFromMaster()
{
	// only if slave
	if (sharedtable->is_master.Get()) return;
	this->to_stop.Set(false);

	::identt::hrpc::HrpcClient hclient;
	::identt::store::TransListT data;
	::identt::store::StoreTrans storetrans;
	::identt::store::TransactionT trans;

	while(!to_stop.Get()) {
		DLOG(INFO) << "update at logc: " << sharedtable->logcounter.Get();
		data.Clear();
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
		}
		if (!status) LOG(INFO) << "Missed an update at logc: " << sharedtable->logcounter.Get();
		std::this_thread::sleep_for( std::chrono::milliseconds( (data.trans_size()>0) ? 50 : 500 ) );
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
	bool status = hclient.SendToRemote(sharedtable,address,::identt::hrpc::R_READONE,&trans1,true);
	if (!status) throw identt::InitialException("Cannot reach master for verification");
	if (trans1.notfound()) throw identt::InitialException("Master does not have last log, mismatch");

	// get the same locally
	::identt::store::TransactionT trans2;
	trans2.set_id( old_logc-1 );
	storetrans.ReadOne(sharedtable,&trans2);
	if (trans2.notfound())
		throw identt::InitialException("This instance does not have last log, mismatch");
	return (trans1.ts() == trans2.ts());
}

