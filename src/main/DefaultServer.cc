/**
 * @project identt
 * @file src/main/DefaultServer.cc
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
 *  DefaultServer.cc :   DefaultServer File
 *
 */

#include <sodium.h>
#define STRIP_FLAG_HELP 1
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <iostream>

/* GFlags Settings Start */
static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	bool status = ( value[0] != '\0' );
	return status;
}

DEFINE_string(config, "", "Config file");
DEFINE_bool(daemon, false, "Start as daemon");

DEFINE_validator(config, &IsNonEmptyMessage);

/* GFlags Settings End */

#include <utils/BaseUtils.hpp>

#include <exception>
#include <stdexcept>
#include <boost/asio/signal_set.hpp>
#include <async++.h>

#include <utils/CfgFileOptions.hpp>
#include <utils/SharedTable.hpp>


#include "DefaultServer.hh"
#include "../work/WorkServer.hpp"

#include "../http/WebServer.hpp"

#ifdef IDENTT_BUILD_WITH_SSL
#include "../http/SecureWebServer.hpp"
#endif

#define IDENTT_DEFAULT_STRN_SYSTEM "system"

/** main */
int main(int argc, char *argv[])
{

	// log only to stderr changed to log also
	if (FLAGS_logtostderr==1) {
		FLAGS_logtostderr=0;
		FLAGS_alsologtostderr=1;
	}
	if (sodium_init() < 0) {
		throw identt::InitialException("sodium cannot initialize, exiting...");
	}

	/** GFlags **/
	std::string usage("The program does nothing.  Sample usage:\n");
	usage += std::string(argv[0]) + " -config my.conf" ;
	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(IDENTT_DEFAULT_EXE_VERSION);
	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	/** Logging **/
	google::InitGoogleLogging(argv[0]);

	/** definitions */
	identt::utils::CfgFileOptions::pointer MyCFG;
	auto wks_section = identt::work::WorkServer::GetSection();
	auto wks_params = identt::work::WorkServer::GetRequire();

	auto wbs_section = identt::http::WebServer::GetSection();
	auto wbs_params = identt::http::WebServer::GetRequire();

#ifdef IDENTT_BUILD_WITH_SSL
	bool sws_enabled = true;
	auto sws_section = identt::http::SecureWebServer::GetSection();
	auto sws_params = identt::http::SecureWebServer::GetRequire();
#endif

	identt::utils::SharedTable::iopointer m_io_service = std::make_shared<boost::asio::io_service>();

	try {
		MyCFG = identt::utils::CfgFileOptions::create(FLAGS_config);

		/** Workserver Params OK */
		for (auto& p : wks_params) p=MyCFG->Find<std::string>(wks_section,p);

		/** Webserver Params OK */
		for (auto& p : wbs_params) p=MyCFG->Find<std::string>(wbs_section,p);
#ifdef IDENTT_BUILD_WITH_SSL
		/** Secure Webserver Params OK */
		if (MyCFG->Check(sws_section,"enabled")) sws_enabled = MyCFG->Find<bool>(sws_section,"enabled");
		if (sws_enabled) {
			for (auto& p : sws_params) p=MyCFG->Find<std::string>(sws_section,p);
		}
#endif


		/** Logging, this point onwards stderr is not there */
		if (MyCFG->Check(IDENTT_DEFAULT_STRN_SYSTEM, "accesslog")) {
			google::SetLogDestination(
			    google::GLOG_INFO, MyCFG->Find<std::string>(IDENTT_DEFAULT_STRN_SYSTEM, "accesslog").c_str() );
		}
		if (MyCFG->Check(IDENTT_DEFAULT_STRN_SYSTEM, "errorlog")) {
			google::SetLogDestination(
			    google::GLOG_ERROR, MyCFG->Find<std::string>(IDENTT_DEFAULT_STRN_SYSTEM, "errorlog").c_str() );
			google::SetLogDestination(
			    google::GLOG_FATAL, MyCFG->Find<std::string>(IDENTT_DEFAULT_STRN_SYSTEM, "errorlog").c_str() );
		}


		/** If not exited Start All Servers */

		auto stptr = identt::utils::SharedTable::create();
		stptr->setIO( m_io_service );
		auto wks = identt::work::WorkServer::create(stptr->share());
		wks->init(wks_params);

		auto wbs = identt::http::WebServer::create(stptr->share());
		wbs->init(wbs_params);

#ifdef IDENTT_BUILD_WITH_SSL
		auto sws = identt::http::SecureWebServer::create(stptr->share());
		if (sws_enabled) {
			sws->init(sws_params);
		}
#endif

		DLOG(INFO) << "STARTING" << std::endl;
		boost::asio::io_service::work m_work(*m_io_service);
		boost::asio::signal_set m_signals(*m_io_service,SIGINT,SIGTERM,SIGHUP);
		// m_signals.async_wait(std::bind(&boost::asio::io_service::stop, m_io_service));
		m_signals.async_wait(
		[&] (const boost::system::error_code& e, int signal_no) {
			wks->stop();
			wbs->stop();
#ifdef IDENTT_BUILD_WITH_SSL
			if (sws_enabled) sws->stop();
#endif
			m_io_service->stop();
		}
		);
		m_io_service->run();
		/** Interrupted */
		DLOG(INFO) << "Interrupted " << std::endl;

	} catch(identt::JdException& e) {
		std::cerr << e.what() << std::endl;
		DLOG(INFO) << "Main Exit " << e.what() << std::endl;
	} catch(std::exception& e) {
		std::cerr << "Error " << e.what() << std::endl;
		DLOG(INFO) << "Main Exit " << e.what() << std::endl;
	} catch(...) {
		DLOG(INFO) << " Unknown Exit" << std::endl;
	}

	gflags::ShutDownCommandLineFlags();
	DLOG(INFO) << "EXITING" << std::endl;
	return 0;
}

