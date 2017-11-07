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
#include <sodium.h>
#define STRIP_FLAG_HELP 1
#include <gflags/gflags.h>
#include <iostream>

/* GFlags Settings Start */
static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	bool status = ( value[0] != '\0' );
	return status;
}

DEFINE_string(shared_secret, "", "shared secret between servers");
DEFINE_string(httphost, "localhost:9091", "host and port");
DEFINE_validator(shared_secret, &IsNonEmptyMessage);
/* GFlags Settings End */

#include "GetMail.hh"

#include <query/SignedJson.hpp> // includes querybase
#include <crypto/CryptoTraits.hpp>
#include <cpr/cpr.h>

/** main */
int main(int argc, char *argv[])
{

	if (sodium_init() < 0) {
		throw identt::InitialException("sodium cannot initialize, exiting...");
	}

	/** GFlags **/
	std::string usage("The program gets email sms with signature.  Sample usage:\n");
	usage += std::string(argv[0]) + " -shared_secret xxxx -httpost localhost:9091" ;
	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(IDENTT_DEFAULT_EXE_VERSION);
	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	/** Logging **/
	google::InitGoogleLogging(argv[0]);
	std::string endpoint = "http://" + FLAGS_httphost + "/_identt/identity/api/v1/getmailstosend";
	identt::mail::MailQueryT mailq;

	try {
		mailq.set_lastid(0);
		mailq.set_limit(10);
		mailq.set_shared_secret(FLAGS_shared_secret);
		std::string tmpstr;
		::identt::query::pb2json(&mailq,tmpstr);
		auto r = cpr::Post(
		             cpr::Url{endpoint},
		             cpr::Body{tmpstr},
		cpr::Header{
			{"Content-Type", "application/json"},
			{"Shared-Secret", FLAGS_shared_secret}
		});
		std::cerr << "Status Code: " << r.status_code << std::endl;
		std::cerr << "Text: " << r.text << std::endl;

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

