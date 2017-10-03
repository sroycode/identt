/**
 * @project identt
 * @file include/http/ServerRequest.hpp
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
 *  ServerRequest.hpp :  HTTP Server Base ( Modified from https://github.com/eidheim/Simple-Web-Server )
 *  - webservice is single threaded
 *
 */
#ifndef _IDENTT_HTTP_SERVER_REQUEST_HPP_
#define _IDENTT_HTTP_SERVER_REQUEST_HPP_

#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <sstream>
#ifdef USE_BOOST_REGEX
#include <boost/regex.hpp>
#define REGEX_NS boost
#else
#include <regex>
#define REGEX_NS std
#endif

#include "ServerContent.hpp"

namespace identt {
namespace http {

struct ServerRequest {
	using Content=ServerContent;
	//Based on http://www.boost.org/doc/libs/1_60_0/doc/html/unordered/hash_equality.html
	struct iequal_to {
		bool operator()(const std::string &key1, const std::string &key2) const
		{
			return boost::algorithm::iequals(key1, key2);
		}
	};
	struct ihash {
		size_t operator()(const std::string &key) const
		{
			std::size_t seed=0;
			for(auto &c: key)
				boost::hash_combine(seed, std::tolower(c));
			return seed;
		}
	};
	ServerRequest(): content(streambuf) {}
	std::string remote_endpoint_address;
	unsigned short remote_endpoint_port;
	std::string method, path, http_version;
	Content content;
	std::unordered_multimap<std::string, std::string, ihash, iequal_to> header;
	REGEX_NS::smatch path_match;
	boost::asio::streambuf streambuf;
};
} // namespace http
} // namespace identt
#endif	/* _IDENTT_HTTP_SERVER_REQUEST_HPP_ */
