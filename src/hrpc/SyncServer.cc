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
#include <async++.h>
#include <functional>
#include <store/StoreTrans.hpp>
#include "../proto/Query.pb.h"
#include "../proto/Store.pb.h"

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
	return {
		"host",
		"port",
		"hrpcurl"
	};
}


/**
* init : initialize rocksdb and others
*
*/
void identt::hrpc::SyncServer::init(identt::utils::ServerBase::ParamsListT params)
{

	try {
		std::string host = params[0];
		std::size_t port = std::stoul(params[1]);
		std::string hrpcurl = params[2];

		if (!sharedtable->is_master.Get()) {
			// see if the pointed machine is master or refers to a master
			::identt::query::StateT sstate;
			size_t tolcount=0;
			::identt::hrpc::HrpcClient hclient;
			do {
				hclient.SendToMaster(sharedtable,"info",&sstate);
				if (!sstate.is_master())
					sharedtable->master.Set( sstate.master() );
				if (++tolcount>2)
					throw identt::InitialException("the pointed machine is not 1 hop away from master");
			} while(!sstate.is_master());
			LOG(INFO) << "Master is set to " << sharedtable->master.Get();
		}

	} catch (identt::JdException& e) {
		LOG(INFO) << "SyncServer init failed: " << e.what() << std::endl;
	}
	DLOG(INFO) << "SyncServer LOOP Ends" << std::endl;
}

/**
* stop : shutdown
*
*/
void identt::hrpc::SyncServer::stop()
{
	DLOG(INFO) << "Sync Server Stop Done" << std::endl;
}

/**
* SyncFromMaster : sync data from master
*
*/
void identt::hrpc::SyncServer::SyncFromMaster(identt::utils::SharedTable::pointer stptr, size_t chunksize)
{
	// only if slave
	if (stptr->is_master.Get()) return;
	::identt::hrpc::HrpcClient hclient;
	::identt::store::TransListT data;
	hclient.SendToMaster(stptr,"TransLog",&data);

}
