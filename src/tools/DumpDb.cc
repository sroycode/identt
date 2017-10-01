/**
 * @project identt
 * @file src/tools/DumpDb.cc
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
 *  DumpDb.cc : DB Dumper
 *
 */
#define STRIP_FLAG_HELP 1
#include <gflags/gflags.h>
#include <iostream>

/* GFlags Settings Start */
static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
	bool status = ( value[0] != '\0' );
	return status;
}

DEFINE_string(path, "", "path to db");
DEFINE_uint64(cachesize, 10, "cache size");
DEFINE_validator(path, &IsNonEmptyMessage);
/* GFlags Settings End */

#include <utils/BaseUtils.hpp>
#include "StoreTablePrint.hpp"

#include "DumpDb.hh"

/** main */
int main(int argc, char *argv[])
{

	/** GFlags **/
	std::string usage("The program dumps db.  Sample usage:\n");
	usage += std::string(argv[0]) + " -path /path/to/db" ;
	gflags::SetUsageMessage(usage);
	gflags::SetVersionString(IDENTT_DEFAULT_EXE_VERSION);
	// read command line
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	/** Logging **/
	google::InitGoogleLogging(argv[0]);

	try {
		::identt::store::StoreTablePrint s{NULL};
		uint64_t ignore;
		s.Initialize(FLAGS_path, FLAGS_cachesize, ignore);
		s.PrintAll();

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

