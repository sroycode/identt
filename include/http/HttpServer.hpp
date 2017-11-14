/**
 * @project identt
 * @file include/http/HttpServer.hpp
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
 *  HttpServer.hpp :  HTTP Server ( Modified from https://github.com/eidheim/Simple-Web-Server )
 *
 */
#ifndef _IDENTT_HTTP_HTTPSERVER_HPP_
#define _IDENTT_HTTP_HTTPSERVER_HPP_

#include "HttpServerBase.hpp"

#ifdef IDENTT_BUILD_WITH_SSL
#include <boost/asio/ssl.hpp>
#endif


namespace identt {
namespace http {

template<class socket_type>
class HttpServer : public HttpServerBase<socket_type> {};
using HTTP=boost::asio::ip::tcp::socket;


template<>
class HttpServer<HTTP> : public HttpServerBase<HTTP> {
public:
	HttpServer(boost::asio::io_service& io_service_,
	           std::string address,unsigned short port,
	           long timeout_request=5,
	           long timeout_content=300) :
		HttpServerBase<HTTP>::HttpServerBase(io_service_,
		                                     address,
		                                     port,
		                                     timeout_request,
		                                     timeout_content) {}
protected:
	void accept()
	{
		//Create new socket for this connection
		//Shared_ptr is used to pass temporary objects to the asynchronous functions
		auto socket=std::make_shared<HTTP>(io_service);
		acceptor->async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
			//Immediately start accepting a new connection (if io_service hasn't been stopped)
			if (ec != boost::asio::error::operation_aborted)
				accept();
			if(!ec) {
				boost::asio::ip::tcp::no_delay option(true);
				socket->set_option(option);
				read_request_and_content(socket);
			}
		});
	}
};

#ifdef IDENTT_BUILD_WITH_SSL
using HTTPS=boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
template<>
class HttpServer<HTTPS> : public HttpServerBase<HTTPS> {
public:
	HttpServer(std::string address, unsigned short port, const std::string& cert_file, const std::string& private_key_file,
	           long timeout_request=5, long timeout_content=300,
	           const std::string& verify_file=std::string()) :
		HttpServerBase<HTTPS>::HttpServerBase(address,port, timeout_request, timeout_content),
		context(boost::asio::ssl::context::tlsv12)   // 2016/08/13 only use tls12, see https://www.ssllabs.com/ssltest
	{
		context.use_certificate_chain_file(cert_file);
		context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);

		if(verify_file.size()>0)
			context.load_verify_file(verify_file);
	}

protected:
	boost::asio::ssl::context context;

	void accept()
	{
		//Create new socket for this connection
		//Shared_ptr is used to pass temporary objects to the asynchronous functions
		auto socket=std::make_shared<HTTPS>(io_service, context);

		acceptor->async_accept((*socket).lowest_layer(), [this, socket](const boost::system::error_code& ec) {
			//Immediately start accepting a new connection (if io_service hasn't been stopped)
			if (ec != boost::asio::error::operation_aborted)
				accept();
			if(!ec) {
				boost::asio::ip::tcp::no_delay option(true);
				socket->lowest_layer().set_option(option);

				//Set timeout on the following boost::asio::ssl::stream::async_handshake
				auto timer=get_timeout_timer(socket, timeout_request);
				(*socket).async_handshake(boost::asio::ssl::stream_base::server, [this, socket, timer]
				(const boost::system::error_code& ec) {
					if(timer)
						timer->cancel();
					if(!ec)
						read_request_and_content(socket);
				});
			}
		});
	}
};
#endif

} // namespace http
} // namespace identt
#endif	/* _IDENTT_HTTP_HTTPSERVER_HPP_ */
