/**
 * @project identt
 * @file include/hrpc/ProtoServiceBase.hpp
 * @author  S Roychowdhury
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018 S Roychowdhury
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
 *  ProtoServiceBase.hpp :   Service ABC
 *
 */
#ifndef _IDENTT_HRPC_PROTO_SERVICE_BASE_HPP_
#define _IDENTT_HRPC_PROTO_SERVICE_BASE_HPP_

#include "utils/BaseUtils.hpp"
#include "../proto/Hrpc.pb.h"

namespace identt {
namespace hrpc {

template <class HttpServerT>
class ProtoServiceBase {
protected:

	/**
	* ProtoRequest : Check if request is protobuf from Content-Type and Accept fields
	*
	* @param request
	*   typename HttpServerT::ReqPtr request
	*
	* @return
	*   bool is proto
	*/
	bool ProtoRequest(typename HttpServerT::ReqPtr request)
	{
		auto it=request->header.find("Content-Type");
		if(it==request->header.end())it=request->header.find("Accept");
		if(it==request->header.end()) return false;
		if ( it->second.compare(0,17,"application/proto",17)!=0) return false;
		it=request->header.find("Proto-Transfer-Encoding");
		return ( it->second.compare(0,6,"base64",6)==0);
	}

	/**
	* SharedSecretOK : Check if shared key header is present and ok
	*
	* @param request
	*   typename HttpServerT::ReqPtr request
	*
	* @param sharedkey
	*   std::string shared key to compare
	*
	* @return
	*   bool is valid
	*/
	bool SharedSecretOK(typename HttpServerT::ReqPtr request, std::string sharedkey)
	{
		auto it=request->header.find("Shared-Secret");
		if(it==request->header.end()) return false;
		return ( it->second == sharedkey );
	}

	/**
	* ServiceName : Check protobuf request service name
	*
	* @param request
	*   typename HttpServerT::ReqPtr request
	*
	* @return
	*   int service id
	*/
	int ServiceName(typename HttpServerT::ReqPtr request)
	{
		auto it=request->header.find("Service-Name");
		return (it==request->header.end()) ?  0 : std::stoi(it->second);
	}

	/**
	* ServiceOKAction : OK Action template for service
	*
	* @param response
	*   typename HttpServerT::RespPtr response
	*
	* @param request
	*   typename HttpServerT::ReqPtr request
	*
	* @param payload
	*   std::string& payload
	*
	* @return
	*   none
	*/
	void ServiceOKAction(
	    typename HttpServerT::RespPtr response,
	    typename HttpServerT::ReqPtr request,
	    std::string& payload)
	{
		*response << "HTTP/" << request->http_version << " 200 OK\r\n";
		*response << "Service-Name: " << ServiceName(request) << "\r\n";
		*response << "Content-Type: " << "application/proto" << "\r\n";
		*response << "Proto-Transfer-Encoding: " << "base64" << "\r\n";
		*response << "Content-Length: " << payload.length() << "\r\n";
		*response << "\r\n" << payload.c_str();
	}

	/**
	* ServiceOKAction : OK Action template for service
	*
	* @param response
	*   typename HttpServerT::RespPtr response
	*
	* @param request
	*   typename HttpServerT::ReqPtr request
	*
	* @param payload
	*   std::string&& payload
	*
	* @return
	*   none
	*/
	void ServiceOKAction(
	    typename HttpServerT::RespPtr response,
	    typename HttpServerT::ReqPtr request,
	    std::string&& payload)
	{
		*response << "HTTP/" << request->http_version << " 200 OK\r\n";
		*response << "Service-Name: " << ServiceName(request) << "\r\n";
		*response << "Content-Type: " << "application/proto" << "\r\n";
		*response << "Proto-Transfer-Encoding: " << "base64" << "\r\n";
		*response << "Content-Length: " << payload.length() << "\r\n";
		*response << "\r\n" << payload.c_str();
	}

	/**
	* ServiceErrAction : Error Action template for service
	*
	* @param response
	*   typename HttpServerT::RespPtr response
	*
	* @param request
	*   typename HttpServerT::ReqPtr request
	*
	* @param ecode
	*   int error code
	*
	* @param emsg
	*   std::string& error message
	*
	* @return
	*   none
	*/
	void ServiceErrAction(
	    typename HttpServerT::RespPtr response,
	    typename HttpServerT::ReqPtr request,
	    int ecode, std::string& emsg)
	{
		*response << "HTTP/" << request->http_version << " 200 OK\r\n";
		*response << "Service-Name: " << ServiceName(request) << "\r\n";
		*response << "Service-Error: " << ecode << "\r\n";
		*response << "Content-Type: application/proto" << "\r\n";
		*response << "Content-Length: " << emsg.length() << "\r\n";
		*response << "\r\n" << emsg.c_str();
	}

};
} // namespace hrpc
} // namespace identt

#endif /* _IDENTT_HRPC_PROTO_SERVICE_BASE_HPP_ */
