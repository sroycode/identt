/**
 * @project identt
 * @file src/http/WebServer.cc
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
 *  WebServer.cc :   Web Server Implementation
 *
 */
#include <syslog.h>
#include <unistd.h>
#include <memory>

#include "WebServer.hpp"
#include <query/WebServiceList.hpp>

identt::http::WebServer::WebServer(std::shared_ptr<::identt::http::io_whatever> io_whatever, identt::utils::SharedTable::pointer stptr)
	: identt::utils::ServerBase(stptr), io_whatever(io_whatever), is_init(false)
{
	DLOG(INFO) << "WebServer Created" << std::endl;
}

identt::http::WebServer::~WebServer ()
{
	DLOG(INFO) << "WebServer Deleted" << std::endl;
}

const std::string identt::http::WebServer::GetSection()
{
	return "http";
}

const identt::utils::ServerBase::ParamsListT identt::http::WebServer::GetRequire()
{
	return {"host","port"};
}

void identt::http::WebServer::init(identt::utils::ServerBase::ParamsListT params)
{
	if (params.size() != this->GetRequire().size() )
		throw identt::ConfigException("WebServer: params and required size mismatch");
	server = std::make_shared<HttpServerT>( std::stoul(params[1]) );
	server->config.address=params[0];
	server->io_whatever = io_whatever;
	is_init=true;
	DLOG(INFO) << "WebServer init 1 here" << std::endl;
	{
		IDENTT_WEBSERVICELIST_SCOPE_HTTP
	}
	DLOG(INFO) << "WebServer init 2 here" << std::endl;
	server->start();
}

void identt::http::WebServer::stop()
{
	if (is_init) server->stop();
	DLOG(INFO) << "WebServer Stopped" << std::endl;
}
