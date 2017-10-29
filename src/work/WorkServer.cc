/**
 * @project identt
 * @file src/work/WorkServer.cc
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
 *  WorkServer.cc :  Work Service
 *
 */
#include "WorkServer.hpp"
#include <store/StoreLevel.hpp>
#include <crypto/CryptoTraits.hpp>
#include <query/bin2ascii.h>

#ifndef IDENTT_USE_SEPARATE_LOGDB
#define IDENTT_USE_SEPARATE_LOGDB 0
#endif

identt::work::WorkServer::WorkServer(identt::utils::SharedTable::pointer stptr)
	: identt::utils::ServerBase(stptr)
{
	DLOG(INFO) << "WorkServer Created" << std::endl;
}

identt::work::WorkServer::~WorkServer ()
{
	DLOG(INFO) << "WorkServer Deleted" << std::endl;
}

const std::string identt::work::WorkServer::GetSection()
{
	return "work";
}

const identt::utils::ServerBase::ParamsListT identt::work::WorkServer::GetRequire()
{
	return {
		"datadir",
		"logdatadir",
		"cachesize",
		"logcachesize",
		"hostseed_ed25519"
	};
}


void identt::work::WorkServer::init(identt::utils::ServerBase::ParamsListT params)
{

	try {
		std::string datadir = params[0];
		std::string logdatadir = params[1];
		std::size_t cachesize = std::stoul(params[2]);
		std::size_t logcachesize = std::stoul(params[3]);
		std::string hostseed = params[4];


		// validations
		if (datadir.empty()) throw identt::InitialException("datadir is needed");
		if (IDENTT_USE_SEPARATE_LOGDB && (logdatadir.empty()))
			throw identt::InitialException("logdatadir is needed");

		// db setup
		uint64_t last_pkey=0;
		uint64_t last_lkey=0;
		identt::store::StoreLevel s{NULL};
		s.Initialize(datadir,cachesize,last_pkey,last_lkey);
		sharedtable->maincounter.Set ( (last_pkey>0) ? last_pkey : 1 );
		sharedtable->maindb.Set( s.getDB() );

		// logdb
		if (IDENTT_USE_SEPARATE_LOGDB) {
			identt::store::StoreLevel p{NULL};
			p.Initialize(logdatadir,logcachesize,last_pkey,last_lkey);
			sharedtable->logdb.Set( p.getDB() );
		} else {
			sharedtable->logdb.Set( s.getDB() );
		}

		// if resets last_lkey
		sharedtable->logcounter.Set ( (last_lkey>0) ? last_lkey : 1 );

		// init Ed25519:0 key
		sharedtable->keyring[ THREEPID_DEFAULT_ALGO_WITH_ID ] =
		    ::identt::crypto::CryptoTraits::CreateFromSeed(THREEPID_DEFAULT_ALGO,hostseed);
	
	} catch (identt::JdException& e) {
		LOG(INFO) << "WorkServer init failed: " << e.what() << std::endl;
		std::rethrow_exception(std::current_exception());
	}
	DLOG(INFO) << "WorkServer LOOP Ends" << std::endl;
}

void identt::work::WorkServer::stop()
{
	DLOG(INFO) << "Work Server Stop Done" << std::endl;
}

