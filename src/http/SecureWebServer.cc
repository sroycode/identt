/**
 * @project identt
 * @file src/http/SecureWebServer.cc
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
 *  SecureWebServer.cc :   Web Server Implementation
 *
 */
#include <syslog.h>
#include <unistd.h>
#include <memory>

#include "SecureWebServer.hpp"
#include <query/WebServiceList.hpp>

identt::http::SecureWebServer::SecureWebServer(boost::asio::io_service& io_service, identt::utils::SharedTable::pointer stptr)
	: identt::utils::ServerBase(stptr),io_service(io_service_),is_init(false)
{
	DLOG(INFO) << "SecureWebServer Created" << std::endl;
}

identt::http::SecureWebServer::~SecureWebServer ()
{
	DLOG(INFO) << "SecureWebServer Deleted" << std::endl;
}

const std::string identt::http::SecureWebServer::GetSection()
{
	return "https";
}

const identt::utils::ServerBase::ParamsListT identt::http::SecureWebServer::GetRequire()
{
	return {"host","port","server_crt","server_key"};
}

void identt::http::SecureWebServer::init(identt::utils::ServerBase::ParamsListT params)
{
	if (params.size() != this->GetRequire().size() )
		throw identt::ConfigException("SecureWebServer: params and required size mismatch");
	DLOG(INFO) << "SecureWebServer Started" << std::endl;

	server = std::make_shared<HttpServerT>(io_service,
	             params[0], // host
	             std::stoul(params[1]), // port
	             params[2], // server_crt
	             params[3] // server_key
	         );
	is_init=true;
	{
		IDENTT_WEBSERVICELIST_SCOPE_HTTPS
	}
	server->start();
}

void identt::http::SecureWebServer::stop()
{
	if (is_init) server->stop();
	DLOG(INFO) << "SecureWebServer Stopped" << std::endl;
}
