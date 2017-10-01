/**
 * @project identt
 * @file include/http/HttpClient.hpp
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
 *  HttpClient.hpp : HTTP Client ( Modified from https://github.com/eidheim/Simple-Web-Server)
 *
 */
#ifndef _IDENTT_HTTP_HTTPCLIENT_HPP_
#define _IDENTT_HTTP_HTTPCLIENT_HPP_

#include "HttpClientBase.hpp"

#ifdef IDENTT_BUILD_WITH_SSL
#include <boost/asio/ssl.hpp>
#endif

namespace identt {
namespace http {

template<class socket_type>
class HttpClient : public ClientBase<socket_type> {};

typedef boost::asio::ip::tcp::socket HTTP;

template<>
class HttpClient<HTTP> : public ClientBase<HTTP> {
public:
	HttpClient(const std::string& server_port_path) : ClientBase<HTTP>::ClientBase(server_port_path, 80)
	{
		socket=std::make_shared<HTTP>(asio_io_service);
	}

private:
	void connect()
	{
		if(socket_error || !socket->is_open()) {
			boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
			boost::asio::connect(*socket, asio_resolver.resolve(query));

			boost::asio::ip::tcp::no_delay option(true);
			socket->set_option(option);

			socket_error=false;
		}
	}
};

#ifdef IDENTT_BUILD_WITH_SSL
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> HTTPS;
template<>
class HttpClient<HTTP> : public ClientBase<HTTPS> {
public:
	HttpClient(const std::string& server_port_path, bool verify_certificate=true,
	           const std::string& cert_file=std::string(), const std::string& private_key_file=std::string(),
	           const std::string& verify_file=std::string()) :
		ClientBase<HTTPS>::ClientBase(server_port_path, 443), asio_context(boost::asio::ssl::context::sslv23)
	{
		if(verify_certificate)
			asio_context.set_verify_mode(boost::asio::ssl::verify_peer);
		else
			asio_context.set_verify_mode(boost::asio::ssl::verify_none);

		if(cert_file.size()>0 && private_key_file.size()>0) {
			asio_context.use_certificate_chain_file(cert_file);
			asio_context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
		}

		if(verify_file.size()>0)
			asio_context.load_verify_file(verify_file);

		socket=std::make_shared<HTTPS>(asio_io_service, asio_context);
	};

private:
	boost::asio::ssl::context asio_context;

	void connect()
	{
		if(socket_error || !socket->lowest_layer().is_open()) {
			boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
			boost::asio::connect(socket->lowest_layer(), asio_resolver.resolve(query));

			boost::asio::ip::tcp::no_delay option(true);
			socket->lowest_layer().set_option(option);

			socket->handshake(boost::asio::ssl::stream_base::client);
			socket_error=false;
		}
	}
};
#endif

} // namespace webserver
} // namespace identt
#endif	/* _IDENTT_HTTP_HTTPCLIENT_HPP_ */
