/**
 * @project identt
 * @file src/hrpc/HrpcClient.cc
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
 *  HrpcClient.cc : Hrpc implementation
 *                   - right now HTTP based with some headers
 *                   - can change to another protocol in future
 *
 */

#include <query/SydentQuery.hpp> // define on top
#include <hrpc/HrpcClient.hpp>
#include <google/protobuf/descriptor.h>
#include "../proto/Query.pb.h"
#include <cpr/cpr.h>

/**
* constructor
*
*/
::identt::hrpc::HrpcClient::HrpcClient() {}

/**
* destructor
*/
::identt::hrpc::HrpcClient::~HrpcClient () {}

/**
* SendToMaster : send to master and get output
*
*/
bool ::identt::hrpc::HrpcClient::SendToMaster(
    ::identt::utils::SharedTable::pointer stptr, std::string service_name,
    google::protobuf::Message* msg)
{
	std::string endpoint = "http://" + stptr->master.Get() + "/_identt/identity/master/v1/endpoint";
	std::string tmpstr;
	msg->SerializeToString(&tmpstr);

	auto r = cpr::Post(
	             cpr::Url{endpoint},
	             cpr::Body{tmpstr},
	cpr::Header{
		{"Service-Name", service_name},
		{"Content-Type", "application/proto"},
		{"Shared-Secret", stptr->shared_secret.Get()}
	});
	if (r.status_code != 200)
		throw identt::BadDataException("Bad HTTP Status");

	if (r.header["Service-Error"].length()>0) {
		int ecode = std::stoi(r.header["Service-Error"]);
		if (ecode>=IDENTT_SYDENT_ERROR_MIN && ecode<=IDENTT_SYDENT_ERROR_MAX)
			throw ::identt::query::SydentException(r.text.c_str(), ecode);
		else
			throw ::identt::BadDataException(r.text.c_str(), M_UNKNOWN);
		// rethrow the error from other side
	}
	if (!msg->ParseFromString(r.text))
		throw identt::BadDataException("Bad Received Protobuf Data Format");
	return true;
}

