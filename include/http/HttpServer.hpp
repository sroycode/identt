/**
 * @project identt
 * @file include/http/HttpServer.hpp
 * @author  S Roychowdhury < sroycode at gmail dot com >
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018-2019 S Roychowdhury
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
 *  HttpServer.hpp :  HTTP Server  ( Modified from https://github.com/eidheim/Simple-Web-Server )
 *
 */
#ifndef _IDENTT_HTTP_SERVER_HPP_
#define _IDENTT_HTTP_SERVER_HPP_

#include "HttpServerBase.hpp"

namespace identt {
namespace http {

template <class socket_type>
class HttpServer : public HttpServerBase<socket_type> {};

using HTTP = asio::ip::tcp::socket;

template <>
class HttpServer<HTTP> : public HttpServerBase<HTTP> {
public:
	HttpServer(unsigned short port) noexcept
		: HttpServerBase<HTTP>::HttpServerBase(port) {}

protected:
	void accept() override
	{
		auto connection = create_connection(*io_whatever);

		acceptor->async_accept(*connection->socket, [this, connection](const error_code &ec) {
			auto lock = connection->handler_runner->continue_lock();
			if(!lock)
				return;

			// Immediately start accepting a new connection (unless io_service has been stopped)
			if(ec != error::operation_aborted)
				this->accept();

			auto session = std::make_shared<Session>(config.max_request_streambuf_size, connection);

			if(!ec) {
				asio::ip::tcp::no_delay option(true);
				error_code ec;
				session->connection->socket->set_option(option, ec);

				this->read(session);
			}
			else if(this->on_error)
				this->on_error(session->request, ec);
		});
	}
};

} // namespace http
} // namespace identt
#endif	/* _IDENTT_HTTP_SERVER_HPP_ */
