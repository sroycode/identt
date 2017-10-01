/**
 * @project identt
 * @file src/http/NotFoundService.hpp
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
 *  NotFoundService.hpp :   NotFound Server Implementation
 *
 */
#ifndef _IDENTT_HTTP_NOTFOUNDSERVICE_HPP_
#define _IDENTT_HTTP_NOTFOUNDSERVICE_HPP_

#include <http/ServiceBase.hpp>

namespace identt {
namespace http {

template <class HttpServerT>
class NotFoundService : protected identt::http::ServiceBase<HttpServerT> {
public:

	/**
	* NotFoundService : constructor
	*
	* @param context
	*   identt::utils::SharedTable::pointer stptr
	*
	* @param server
	*   HttpServerT server
	*
	* @param scope
	*   unsigned int scope check
	*
	* @return
	*   none
	*/

	NotFoundService(identt::utils::SharedTable::pointer stptr, typename std::shared_ptr<HttpServerT> server, unsigned int scope)
		: identt::http::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch
		server->default_resource["GET"] =
		[this](std::shared_ptr<typename HttpServerT::Response> response, std::shared_ptr<typename HttpServerT::Request> request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};
		server->default_resource["POST"] =
		[this](std::shared_ptr<typename HttpServerT::Response> response, std::shared_ptr<typename HttpServerT::Request> request) {
			this->HttpErrorAction(response,request,404,"NOT FOUND");
		};
	}

private:
};

} // namespace http
} // namespace identt

#endif // _IDENTT_HTTP_NOTFOUNDSERVICE_HPP_
