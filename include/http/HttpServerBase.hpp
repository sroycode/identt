/**
 * @project identt
 * @file include/http/HttpServerBase.hpp
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
 *  HttpServerBase.hpp :  HTTP Server Base ( Modified from https://github.com/eidheim/Simple-Web-Server )
 *  - webservice is single threaded
 *
 */
#ifndef _IDENTT_HTTP_SERVER_BASE_HPP_
#define _IDENTT_HTTP_SERVER_BASE_HPP_

#include <functional>
#include <iostream>

#include "ServerContent.hpp"
#include "ServerRequest.hpp"
#include "ServerResponse.hpp"
#include "ServerConfig.hpp"

namespace identt {
namespace http {

template <class socket_type>
class HttpServerBase {
public:
	using Content = ServerContent;
	using Request = ServerRequest;
	using Response = ServerResponse<socket_type>;
	using Config = ServerConfig;
	using RespPtr = std::shared_ptr<Response>;
	using ReqPtr = std::shared_ptr<Request>;
	using RespReqF =  std::function<void(RespPtr,ReqPtr)>;

	virtual ~HttpServerBase() {}

	///Set before calling start().
	Config config;

	std::unordered_map<std::string, std::unordered_map<std::string, RespReqF > >  resource;
	std::unordered_map<std::string, RespReqF > default_resource;
	std::function<void(const std::exception&)> exception_handler;
private:
	std::vector<std::pair<std::string, std::vector<std::pair<REGEX_NS::regex, RespReqF > > > > opt_resource;
public:
	void start()
	{
		//Copy the resources to opt_resource for more efficient request processing
		opt_resource.clear();
		for(auto& res: resource) {
			for(auto& res_method: res.second) {
				auto it=opt_resource.end();
				for(auto opt_it=opt_resource.begin(); opt_it!=opt_resource.end(); opt_it++) {
					if(res_method.first==opt_it->first) {
						it=opt_it;
						break;
					}
				}
				if(it==opt_resource.end()) {
					opt_resource.emplace_back();
					it=opt_resource.begin()+(opt_resource.size()-1);
					it->first=res_method.first;
				}
				it->second.emplace_back(REGEX_NS::regex(res.first), res_method.second);
			}
		}
		if (io_service.stopped()) io_service.reset();
		boost::asio::ip::tcp::endpoint endpoint;
		if(config.address.size()>0)
			endpoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(config.address), config.port);
		else
			endpoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), config.port);
		if(!acceptor)
			acceptor=std::unique_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(io_service));
		acceptor->open(endpoint.protocol());
		acceptor->set_option(boost::asio::socket_base::reuse_address(config.reuse_address));
		acceptor->bind(endpoint);
		acceptor->listen();
		accept();
	}
	void stop()
	{
		acceptor->close();
	}
	///Use this function if you need to recursively send parts of a longer message
	void send(const RespPtr &response, const std::function<void(const boost::system::error_code&)>& callback=nullptr) const
	{
		boost::asio::async_write(*response->socket, response->streambuf, [this, response, callback](const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
			if(callback)
				callback(ec);
		});
	}
protected:
	boost::asio::io_service& io_service;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
	long timeout_request;
	long timeout_content;

	HttpServerBase(boost::asio::io_service& io_service_,
	               std::string address,
	               unsigned short port,
	               long timeout_request,
	               long timeout_send_or_receive) :
		io_service(io_service_),
		config(address, port),
		timeout_request(timeout_request),
		timeout_content(timeout_send_or_receive) {}

	virtual void accept()=0;
	std::shared_ptr<boost::asio::deadline_timer> get_timeout_timer(const std::shared_ptr<socket_type> &socket, long seconds)
	{
		if(seconds==0)
			return nullptr;
		auto timer=std::make_shared<boost::asio::deadline_timer>(io_service);
		timer->expires_from_now(boost::posix_time::seconds(seconds));
		timer->async_wait([socket](const boost::system::error_code& ec) {
			if(!ec) {
				boost::system::error_code ec;
				socket->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				socket->lowest_layer().close();
			}
		});
		return timer;
	}
	void read_request_and_content(const std::shared_ptr<socket_type> &socket)
	{
		//Create new streambuf (Request::streambuf) for async_read_until()
		//shared_ptr is used to pass temporary objects to the asynchronous functions
		ReqPtr request(new Request());
		try {
			request->remote_endpoint_address=socket->lowest_layer().remote_endpoint().address().to_string();
			request->remote_endpoint_port=socket->lowest_layer().remote_endpoint().port();
		} catch(const std::exception &e) {
			if(exception_handler)
				exception_handler(e);
		}
		//Set timeout on the following boost::asio::async-read or write function
		auto timer=get_timeout_timer(socket, timeout_request);
		boost::asio::async_read_until(*socket, request->streambuf, "\r\n\r\n",
		[this, socket, request, timer](const boost::system::error_code& ec, size_t bytes_transferred) {
			if(timer)
				timer->cancel();
			if(!ec) {
				//request->streambuf.size() is not necessarily the same as bytes_transferred, from Boost-docs:
				//"After a successful async_read_until operation, the streambuf may contain additional data beyond the delimiter"
				//The chosen solution is to extract lines from the stream directly when parsing the header. What is left of the
				//streambuf (maybe some bytes of the content) is appended to in the async_read-function below (for retrieving content).
				size_t num_additional_bytes=request->streambuf.size()-bytes_transferred;
				if(!parse_request(request))
					return;
				//If content, read that as well
				auto it=request->header.find("Content-Length");
				if(it!=request->header.end()) {
					unsigned long long content_length;
					try {
						content_length=stoull(it->second);
					} catch(const std::exception &e) {
						if(exception_handler)
							exception_handler(e);
						return;
					}
					if(content_length>num_additional_bytes) {
						//Set timeout on the following boost::asio::async-read or write function
						auto timer=get_timeout_timer(socket, timeout_content);
						boost::asio::async_read(*socket, request->streambuf,
						                        boost::asio::transfer_exactly(content_length-num_additional_bytes),
						                        [this, socket, request, timer]
						(const boost::system::error_code& ec, size_t /*bytes_transferred*/) {
							if(timer)
								timer->cancel();
							if(!ec)
								find_resource(socket, request);
						});
					} else
						find_resource(socket, request);
				} else
					find_resource(socket, request);
			}
		});
	}
	bool parse_request(const ReqPtr &request) const
	{
		std::string line;
		getline(request->content, line);
		size_t method_end;
		if((method_end=line.find(' '))!=std::string::npos) {
			size_t path_end;
			if((path_end=line.find(' ', method_end+1))!=std::string::npos) {
				request->method=line.substr(0, method_end);
				request->path=line.substr(method_end+1, path_end-method_end-1);
				size_t protocol_end;
				if((protocol_end=line.find('/', path_end+1))!=std::string::npos) {
					if(line.substr(path_end+1, protocol_end-path_end-1)!="HTTP")
						return false;
					request->http_version=line.substr(protocol_end+1, line.size()-protocol_end-2);
				} else
					return false;
				getline(request->content, line);
				size_t param_end;
				while((param_end=line.find(':'))!=std::string::npos) {
					size_t value_start=param_end+1;
					if((value_start)<line.size()) {
						if(line[value_start]==' ')
							value_start++;
						if(value_start<line.size())
							request->header.insert(std::make_pair(line.substr(0, param_end), line.substr(value_start, line.size()-value_start-1)));
					}
					getline(request->content, line);
				}
			} else
				return false;
		} else
			return false;
		return true;
	}
	void find_resource(const std::shared_ptr<socket_type> &socket, const ReqPtr &request)
	{
		//Find path- and method-match, and call write_response
		for(auto& res: opt_resource) {
			if(request->method==res.first) {
				for(auto& res_path: res.second) {
					REGEX_NS::smatch sm_res;
					if(REGEX_NS::regex_match(request->path, sm_res, res_path.first)) {
						request->path_match=std::move(sm_res);
						write_response(socket, request, res_path.second);
						return;
					}
				}
			}
		}
		auto it_method=default_resource.find(request->method);
		if(it_method!=default_resource.end()) {
			write_response(socket, request, it_method->second);
		}
	}
	void write_response(const std::shared_ptr<socket_type> &socket, const ReqPtr &request, RespReqF& resource_function)
	{
		//Set timeout on the following boost::asio::async-read or write function
		auto timer=get_timeout_timer(socket, timeout_content);
		auto response=RespPtr(new Response(socket), [this, request, timer](Response *response_ptr) {
			auto response=RespPtr(response_ptr);
			send(response, [this, response, request, timer](const boost::system::error_code& ec) {
				if(timer)
					timer->cancel();
				if(!ec) {
					float http_version;
					try {
						http_version=stof(request->http_version);
					} catch(const std::exception &e) {
						if(exception_handler)
							exception_handler(e);
						return;
					}
					auto range=request->header.equal_range("Connection");
					for(auto it=range.first; it!=range.second; it++) {
						if(boost::iequals(it->second, "close"))
							return;
					}
					if(http_version>1.05)
						read_request_and_content(response->socket);
				}
			});
		});
		try {
			resource_function(response, request);
		} catch(const std::exception &e) {
			if(exception_handler)
				exception_handler(e);
			return;
		}
	}
};

} // namespace http
} // namespace identt
#endif	/* _IDENTT_HTTP_SERVER_BASE_HPP_ */
