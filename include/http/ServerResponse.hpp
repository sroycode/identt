/**
 * @project identt
 * @file include/http/ServerResponse.hpp
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
 *  ServerResponse.hpp :  Server Response
 *
 */
#ifndef _IDENTT_HTTP_SERVER_RESPONSE_HPP_
#define _IDENTT_HTTP_SERVER_RESPONSE_HPP_

#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

namespace identt {
namespace http {

template <class socket_type>
struct ServerResponse : public std::ostream {
	ServerResponse(const std::shared_ptr<socket_type> &socket): std::ostream(&streambuf), socket(socket) {}
	size_t size()
	{
		return streambuf.size();
	}
	boost::asio::streambuf streambuf;
	std::shared_ptr<socket_type> socket;
};

} // namespace http
} // namespace identt
#endif	/* _IDENTT_HTTP_SERVER_RESPONSE_HPP_ */
