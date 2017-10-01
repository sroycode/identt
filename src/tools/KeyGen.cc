/**
 * @project identt
 * @file src/tools/KeyGen.cc
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
 *  KeyGen.cc : Keygen key generator
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

DEFINE_string(key, "ed25519", "key name");
DEFINE_string(seed, "", "seed");
DEFINE_validator(key, &IsNonEmptyMessage);
/* GFlags Settings End */

#include <utils/BaseUtils.hpp>
#include <query/bin2ascii.h>

#include "KeyGen.hh"
#include <crypto/Ed25519.hpp>

class Ed25519_extended : public identt::crypto::Ed25519 {
public:
	using identt::crypto::Ed25519::Ed25519;
	void Print()
	{
		std::cout << "Ed25519 encoding keys:" << std::endl;
		std::cout << "pkey: " <<  GetPublicKey() << std::endl;
		std::cout << "skey: " << GetSecretKey() << std::endl;
		std::cout << "seed: " << GetSeed() << std::endl;
	}
	void Test()
	{
		std::string t= "{\"limit\":10,\"payload\":[]}";
		std::cout << std::endl;
		std::cout << "Testing:" << t << std::endl;
		std::string s = GetSignature(t);
		std::cout << "Signature: " << s << std::endl;
		std::cout << "Status: " << VerifySignature(s,t) << std::endl;
	}
};


/** main */
int main(int argc, char *argv[])
{

	if (sodium_init() < 0) {
		throw identt::InitialException("sodium cannot initialize, exiting...");
	}

	/** GFlags **/
	std::string usage("The program generates keys.  Sample usage:\n");
	usage += std::string(argv[0]) + " -key ed25519 -seed XXXX" ;
	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(IDENTT_DEFAULT_EXE_VERSION);
	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	/** Logging **/
	google::InitGoogleLogging(argv[0]);

	try {
		if (FLAGS_key=="ed25519") {
			if (FLAGS_seed.length()==0) {
				Ed25519_extended mykey;
				mykey.Print();
				mykey.Test();
			} else {
				Ed25519_extended mykey{FLAGS_seed};
				mykey.Print();
				mykey.Test();
			}
		}

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

