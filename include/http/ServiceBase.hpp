/**
 * @project identt
 * @file include/http/ServiceBase.hpp
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
 *  ServiceBase.hpp :   Service ABC
 *
 */
#ifndef _IDENTT_HTTP_SERVICEBASE_HPP_
#define _IDENTT_HTTP_SERVICEBASE_HPP_

#define IDENTT_SERVICE_SCOPE_HTTP  0x0001
#define IDENTT_SERVICE_SCOPE_HTTPS 0x0002


#include <utils/BaseUtils.hpp>
#include <utils/SharedTable.hpp>
#include <fstream>
#include <iomanip>

#define IDENTT_HTTP_CHAR_PERCENT '%'
#define IDENTT_HTTP_CHAR_SPACE ' '
#define IDENTT_HTTP_CHAR_PLUS '+'
#define IDENTT_HTTP_STR_EQUALTO "="
#define IDENTT_HTTP_STR_AMPERSAND "&"

namespace identt {
namespace http {

struct InvalidDataError {
	bool error=false;
	std::string message;
};

template <class HttpServerT>
class ServiceBase {
public:


protected:
	const unsigned int myscope;

	/**
	* Constructor : to be used by inherited classes
	*
	*/
	ServiceBase(const unsigned int myscope_) : myscope(myscope_) {}


	/**
	* HttpErrorAction : Error Action Template no payload maybe template
	*
	* @param response
	*   std::shared_ptr<typename HttpServerT::Response> response
	*
	* @param request
	*   std::shared_ptr<typename HttpServerT::Request> request
	*
	* @param ec
	*   int http error code
	*
	* @param em
	*   const char* http status message
	*
	* @return
	*   none
	*/
	void HttpErrorAction(std::shared_ptr<typename HttpServerT::Response> response, std::shared_ptr<typename HttpServerT::Request> request,
	                     int ec, const char* em)
	{
		std::string output="";
		std::stringstream content_stream;
		content_stream << output;
		content_stream << ec << " " << em << request->method << " " << request->path << " HTTP/" << request->http_version << "";
		content_stream.seekp(0, std::ios::end);
		output = content_stream.str();
		*response <<  "HTTP/" << request->http_version << " " << ec << " " << em;
		*response << "\r\nContent-Length: " << output.length() << "\r\n\r\n" << output.c_str();
	}

	/**
	* HttpErrorAction : Error Action Template with payload no template
	*
	* @param response
	*   std::shared_ptr<typename HttpServerT::Response> response
	*
	* @param request
	*   std::shared_ptr<typename HttpServerT::Request> request
	*
	* @param ec
	*   int http error code
	*
	* @param em
	*   const char* http status message
	*
	* @param payload
	*   std::string payload
	*
	* @return
	*   none
	*/
	void HttpErrorAction(std::shared_ptr<typename HttpServerT::Response> response, std::shared_ptr<typename HttpServerT::Request> request,
	                     int ec, const char* em, std::string payload)
	{
		*response <<  "HTTP/" << request->http_version << " " << ec << " " << em;
		*response << "\r\nContent-Length: " << payload.length() << "\r\n\r\n" << payload.c_str();
	}

	/**
	* HttpOKAction : OK Action template
	*
	* @param response
	*   std::shared_ptr<typename HttpServerT::Response> response
	*
	* @param request
	*   std::shared_ptr<typename HttpServerT::Request> request
	*
	* @param ec
	*   int http error code
	*
	* @param em
	*   const char* http status message
	*
	* @param content_type
	*   const char* content type
	*
	* @param payload
	*   std::string& payload
	*
	* @param add_cors
	*   bool add cors headers
	*
	* @return
	*   none
	*/
	void HttpOKAction(std::shared_ptr<typename HttpServerT::Response> response, std::shared_ptr<typename HttpServerT::Request> request,
	                  int ec, const char* em, const char* content_type , std::string& payload, bool add_cors)
	{
		*response << "HTTP/" << request->http_version << " " << ec <<  " " << em << "\r\n";
		if (add_cors) {
			*response << "Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept\r\n";
			*response << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
			*response << "Access-Control-Allow-Origin: *\r\n";
		}
		*response << "Content-type: " << content_type << "\r\n";
		*response << "Content-Length: " << payload.length() << "\r\n";
		*response << "\r\n" << payload.c_str();
	}

	/**
	* JsonRequest : Check if request is json from Content-Type and Accept fields
	*
	* @param request
	*   std::shared_ptr<typename HttpServerT::Request> request
	*
	* @return
	*   bool is json
	*/
	bool JsonRequest(std::shared_ptr<typename HttpServerT::Request> request)
	{
		auto it=request->header.find("Content-Type");
		if(it==request->header.end()) it=request->header.find("Accept");
		if(it==request->header.end()) return false;
		return ( it->second.compare(0,16,"application/json",16)==0);
	}

};
} // namespace http
} // namespace identt

#endif /* _IDENTT_HTTP_SERVICEBASE_HPP_ */
