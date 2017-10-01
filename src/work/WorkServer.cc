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
#include <query/SydentQuery.hpp>
#include <query/bin2ascii.h>

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
	return {"datadir","cachesize","mail_host","hostseed_ed25519","mailkey_ed25519","base_url"};
}


void identt::work::WorkServer::init(identt::utils::ServerBase::ParamsListT params)
{

	try {
		std::string datadir = params[0];
		std::size_t cachesize = std::stoul(params[1]);
		std::string mailhost = params[2];
		std::string hostseed = params[3];
		std::string mailkey = params[4];
		std::string baseurl = params[5];

		// db setup
		uint64_t last_pkey=0;
		identt::store::StoreLevel s{NULL};
		s.Initialize(datadir,cachesize,last_pkey);
		sharedtable->set_lastkey ( (last_pkey>0) ? last_pkey : 1 );
		sharedtable->setDB( s.getDB() );
		sharedtable->set_mailhost( mailhost );
		sharedtable->set_baseurl( baseurl );

		// init Ed25519:0 key
		sharedtable->keyring[ THREEPID_DEFAULT_ALGO_WITH_ID ] =
		    ::identt::crypto::CryptoTraits::CreateFromSeed(THREEPID_DEFAULT_ALGO,hostseed);
		// init mail sign key Ed25510:mail
		sharedtable->keyring[ THREEPID_EXTMAIL_ALGO_WITH_ID ] =
		    ::identt::crypto::CryptoTraits::Create(THREEPID_DEFAULT_ALGO,IDENTT_CRYPTO_SCOPE_VERIFY,mailkey);

		// final
		sharedtable->set_ready ( true );
	} catch (identt::JdException& e) {
		LOG(INFO) << "WorkServer init failed: " << e.what() << std::endl;
	}
	DLOG(INFO) << "WorkServer LOOP Ends" << std::endl;
}

void identt::work::WorkServer::stop()
{
	DLOG(INFO) << "Work Server Stop Done" << std::endl;
}

