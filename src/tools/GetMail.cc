/**
 * @project identt
 * @file src/tools/GetMail.cc
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
 *  GetMail.cc : Keygen key generator
 *
 */

#define STRIP_FLAG_HELP 1
#include <gflags/gflags.h>

/* GFlags Settings Start */
DEFINE_bool(h, false, "Show help");
DECLARE_bool(help);
DECLARE_bool(helpshort);
static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	bool status = ( value[0] != '\0' );
	return status;
}

DEFINE_string(config, "", "Config file");
DEFINE_validator(config, &IsNonEmptyMessage);

DEFINE_string(shared_secret, "", "shared secret");
DEFINE_string(httphost, "", "http host+port");
DEFINE_uint64(limit,30,"no of queries");
DEFINE_uint64(timeout,3,"timeout in secs");

/* GFlags Settings End */

#include <iostream>
#include <functional>
#include <exception>
#include <future>
#include <deque>

#include "GetMail.hh"
#include <functional>
#include <boost/algorithm/string.hpp>
#include "../proto/Mail.pb.h"
#include <query/ProtoJson.hpp> // contains BaseUtils
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/pointer.h>

#include <utils/CfgFileOptions.hpp>
#include <cpr/cpr.h>

/** main */
int main(int argc, char *argv[])
{

	/** GFlags **/
	std::string usage("The program gets email sms with signature and sends the same.  Sample usage:\n");
	usage += std::string(argv[0]) + " -config ../etc/getmail.conf" ;
	usage += std::string(argv[0]) + " -config ../etc/getmail.conf -httphost http://127.0.0.1:9091" ;
	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(IDENTT_DEFAULT_EXE_VERSION);
	// log only to stderr changed to log also
	if (FLAGS_logtostderr) {
		FLAGS_logtostderr = false;
		FLAGS_alsologtostderr = true;
	}
	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	/** Logging **/
	google::InitGoogleLogging(argv[0]);

	try {
		/** Config Params Start */

		identt::utils::CfgFileOptions::pointer MyCFG;
		MyCFG = identt::utils::CfgFileOptions::create(FLAGS_config);

		// shared secret
		std::string shared_secret;
		if (!FLAGS_shared_secret.empty())
			shared_secret = FLAGS_shared_secret;
		else if (MyCFG->Check("system", "shared_secret"))
			shared_secret = MyCFG->Find<std::string>("system", "shared_secret");
		else
			throw identt::InitialException("shared_secret is needed");

		// httphost
		std::string httphost;
		if (!FLAGS_httphost.empty())
			httphost = FLAGS_httphost;
		else if (MyCFG->Check("system", "httphost"))
			httphost = MyCFG->Find<std::string>("system", "httphost");
		else
			throw identt::InitialException("httphost is needed");

		// authtoken
		const std::string authtoken_template {
			MyCFG->Find<std::string>("sms_template", "authtoken")
		};

		/** Logging, this point onwards stderr is not there */
		if (MyCFG->Check("system", "accesslog")) {
			google::SetLogDestination( google::GLOG_INFO, MyCFG->Find<std::string>("system", "accesslog").c_str() );
		}
		if (MyCFG->Check("system", "errorlog")) {
			google::SetLogDestination( google::GLOG_ERROR, MyCFG->Find<std::string>("system", "errorlog").c_str() );
			google::SetLogDestination( google::GLOG_FATAL, MyCFG->Find<std::string>("system", "errorlog").c_str() );
		}

		/** Config Params End */

		std::string endpoint = httphost + "/_identt/identity/api/v1/getmailstosend";
		identt::mail::MailQueryT mailq;
		uint64_t lastid =0;

		const size_t my_timeout = ( FLAGS_timeout >=1 && FLAGS_timeout <=10) ? FLAGS_timeout*1000 : 3000;
		const size_t my_limit  = ( FLAGS_limit >=1 && FLAGS_limit <=1000) ? FLAGS_limit : 100;
		std::string err;
		const std::string token_pattern{"{token}"};

		bool ok=false;
		do {
			std::vector<std::future<cpr::Response>> container{};

			mailq.set_lastid(lastid);
			mailq.set_limit(my_limit);
			std::string tmpstr;
			::identt::query::pb2json(&mailq,tmpstr);
			auto r = cpr::Post( cpr::Url{endpoint}, cpr::Body{tmpstr}, cpr::Timeout{1000},
			cpr::Header{{"Content-Type", "application/json"},{"Shared-Secret", shared_secret}} );
			if (r.status_code!=200) continue; // error in request
			int stat = ::identt::query::json2pb(r.text,&mailq,err);
			if (stat<0) continue; // error in json
			for (size_t i=0; i<mailq.payload_size(); ++i ) {
				auto payload = mailq.mutable_payload(i);
				switch (payload->actkey()) {
				case ::identt::mail::A_AUTHTOKEN:	{
					if (payload->medium() != "msisdn") break;
					if ( !MyCFG->Check("sms_server", payload->mutable_mpone()->country() )) break;
					auto url = MyCFG->Find<std::string>("sms_server", payload->mutable_mpone()->country() );
					if (payload->medium() != "msisdn") break;
					std::string mytoken = authtoken_template;
					while (mytoken.find(token_pattern) != std::string::npos) {
						mytoken.replace(mytoken.find(token_pattern), token_pattern.length(), payload->mutable_mpone()->token());
					}

					LOG(INFO) << "Sending to " << "+" + payload->address() << std::endl;
					container.emplace_back( cpr::GetAsync( cpr::Url{url},
					cpr::Parameters{ {"mobile", "+" + payload->address()}, {"message", mytoken } },
					cpr::Header{ {"User-Agent", "Sydent"} },
					cpr::Timeout{3000} ) );
					// mark for deletion
					payload->mutable_mpone()->Clear();
					payload->set_actkey( ::identt::mail::A_MAILSENT);
					break;
				}

				case ::identt::mail::A_USERINVITE: {
					// mark for deletion
					payload->set_actkey( ::identt::mail::A_MAILSENT);
					break;
				}
				case ::identt::mail::A_ROOMINVITE: {
					// mark for deletion
					payload->set_actkey( ::identt::mail::A_MAILSENT);
					break;
				}
				case ::identt::mail::A_ONBINDNOTIFY: {
					// mark for deletion
					payload->set_actkey( ::identt::mail::A_MAILSENT);
					break;
				}
				default: {
					// mark for deletion
					payload->set_actkey( ::identt::mail::A_MAILSENT);
					break;
				}
				}
			}

			size_t scount=0;
			for (auto& fr : container) {
				auto fv = fr.get();
				if (fv.text.length()>0) ++scount;
			}
			// dont delete if no content sent out
			if (scount==0 && container.size()>0) mailq.clear_payload();
		}
		while (ok);

	}
	catch(identt::JdException& e) {
		std::cerr << e.what() << std::endl;
		DLOG(INFO) << "Main Exit " << e.what() << std::endl;
	}
	catch(std::exception& e) {
		std::cerr << "Error " << e.what() << std::endl;
		DLOG(INFO) << "Main Exit " << e.what() << std::endl;
	}
	catch(...) {
		DLOG(INFO) << " Unknown Exit" << std::endl;
	}

	gflags::ShutDownCommandLineFlags();
	DLOG(INFO) << "EXITING" << std::endl;
	return 0;
}

