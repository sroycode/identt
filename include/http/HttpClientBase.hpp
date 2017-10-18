/**
 * @project identt
 * @file include/http/HttpClientBase.hpp
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
 *  HttpClientBase.hpp : Http Client base
 *
 */
#ifndef _IDENTT_HTTP_CLIENTBASE_HPP_
#define _IDENTT_HTTP_CLIENTBASE_HPP_

#include <map>
#include <random>
#include "ClientResponse.hpp"

namespace identt {
namespace http {

template <class socket_type>
class ClientBase {
public:
	using Response = ClientResponse<socket_type>;
	using RespPtr = std::shared_ptr<Response>;

	RespPtr request(
	    const std::string& request_type,
	    const std::string& path="/",
	    const std::map<std::string,
	    std::string>& header=std::map<std::string, std::string>())
	{
		std::stringstream empty_ss;
		return request(request_type, path, empty_ss, header);
	}

	RespPtr request(const std::string& request_type, const std::string& path, std::ostream& content,
	                                  const std::map<std::string, std::string>& header=std::map<std::string, std::string>())
	{
		std::string corrected_path=path;
		if(corrected_path=="") corrected_path="/";

		content.seekp(0, std::ios::end);
		size_t content_length=content.tellp();
		content.seekp(0, std::ios::beg);

		boost::asio::streambuf write_buffer;
		std::ostream write_stream(&write_buffer);
		write_stream << request_type << " " << corrected_path << " HTTP/1.1\r\n";
		write_stream << "Host: " << host << "\r\n";
		for(auto& h: header) {
			write_stream << h.first << ": " << h.second << "\r\n";
		}
		if(content_length>0)
			write_stream << "Content-Length: " << std::to_string(content_length) << "\r\n";
		write_stream << "\r\n";
		if(content_length>0)
			write_stream << content.rdbuf();

		RespPtr response(new Response());

		try {
			connect();
			boost::asio::write(*socket, write_buffer);
			size_t bytes_transferred = boost::asio::read_until(*socket, response->content_buffer, "\r\n\r\n");
			size_t num_additional_bytes=response->content_buffer.size()-bytes_transferred;
			parse_response_header(response, response->content);

			if(response->header.count("Content-Length")>0) {
				boost::asio::read(*socket, response->content_buffer,
				                  boost::asio::transfer_exactly(stoull(response->header["Content-Length"])-num_additional_bytes));
			} else if(response->header.count("Transfer-Encoding")>0 && response->header["Transfer-Encoding"]=="chunked") {
				boost::asio::streambuf streambuf;
				std::ostream content(&streambuf);

				size_t length;
				std::string buffer;
				do {
					size_t bytes_transferred = boost::asio::read_until(*socket, response->content_buffer, "\r\n");
					std::string line;
					getline(response->content, line);
					bytes_transferred-=line.size()+1;
					line.pop_back();
					length=stoull(line, 0, 16);

					size_t num_additional_bytes=response->content_buffer.size()-bytes_transferred;

					if((2+length)>num_additional_bytes) {
						boost::asio::read(*socket, response->content_buffer,
						                  boost::asio::transfer_exactly(2+length-num_additional_bytes));
					}

					buffer.resize(length);
					response->content.read(&buffer[0], length);
					content.write(&buffer[0], length);

					//Remove "\r\n"
					response->content.get();
					response->content.get();
				} while(length>0);

				std::ostream response_content_output_stream(&response->content_buffer);
				response_content_output_stream << content.rdbuf();
			}
		} catch(const std::exception& e) {
			socket_error=true;
			throw std::invalid_argument(e.what());
		}

		return response;
	}

protected:
	boost::asio::io_service asio_io_service;
	boost::asio::ip::tcp::endpoint asio_endpoint;
	boost::asio::ip::tcp::resolver asio_resolver;

	std::shared_ptr<socket_type> socket;
	bool socket_error;

	std::string host;
	unsigned short port;

	ClientBase(const std::string& host_port, unsigned short default_port) :
		asio_resolver(asio_io_service), socket_error(false)
	{
		size_t host_end=host_port.find(':');
		if(host_end==std::string::npos) {
			host=host_port;
			port=default_port;
		} else {
			host=host_port.substr(0, host_end);
			port=(unsigned short)stoul(host_port.substr(host_end+1));
		}

		asio_endpoint=boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
	}

	virtual void connect()=0;

	void parse_response_header(RespPtr response, std::istream& stream) const
	{
		std::string line;
		getline(stream, line);
		size_t version_end=line.find(' ');
		if(version_end!=std::string::npos) {
			response->http_version=line.substr(5, version_end-5);
			response->status_code=line.substr(version_end+1, line.size()-version_end-2);

			getline(stream, line);
			size_t param_end=line.find(':');
			while(param_end!=std::string::npos) {
				size_t value_start=param_end+1;
				if(line[value_start]==' ')
					value_start++;

				response->header[line.substr(0, param_end)]=line.substr(value_start, line.size()-value_start-1);

				getline(stream, line);
				param_end=line.find(':');
			}
		}
	}
};

} // namespace webserver
} // namespace identt
#endif	/* _IDENTT_HTTP_CLIENTBASE_HPP_ */
