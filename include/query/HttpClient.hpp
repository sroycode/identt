/**
 * @project identt
 * @file include/query/HttpClient.hpp
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
 *  HttpClient.hpp : Header http client
 *
 */
#ifndef _IDENTT_QUERY_HTTP_CLIENT_HPP_
#define _IDENTT_QUERY_HTTP_CLIENT_HPP_

#include <utils/BaseUtils.hpp>
#include <utils/SharedTable.hpp>
#include <unordered_map>
#include <cpr/cpr.h>

namespace identt {
namespace query {

class HttpClient {
public:

	using GetParamsT=std::unordered_map<std::string,std::string>;
	/**
	* constructor
	*
	*/
	HttpClient();

	/**
	* make noncopyable
	*/
	HttpClient(const HttpClient&) = delete;
	HttpClient& operator=(const HttpClient&) = delete;

	/**
	* destructor
	*/
	virtual ~HttpClient ();

	/**
	* GetDefault : send json to remote and get output
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param url
	*   std::string url
	*
	* @param myparams
	*   cpr::Parameters parameters
	*
	* @param returns
	*   std::string& returns
	*
	* @param nothrow
	*   bool dont throw if true
	*
	* @return
	*   bool status true if ok
	*/
	bool GetDefault(::identt::utils::SharedTable::pointer stptr,
	                std::string url, cpr::Parameters myparams, std::string& returns, bool nothrow=false);

	/**
	* PostJson : send json to remote and get output
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param url
	*   std::string url
	*
	* @param payload
	*   std::string& payload
	*
	* @param returns
	*   std::string& returns
	*
	* @param nothrow
	*   bool dont throw if true
	*
	* @return
	*   bool status true if ok
	*/
	bool PostJson(::identt::utils::SharedTable::pointer stptr,
	              std::string url, std::string& payload, std::string& returns, bool nothrow=false);

};
} // namespace query
} // namespace identt
#endif // _IDENTT_QUERY_HTTP_CLIENT_HPP_

