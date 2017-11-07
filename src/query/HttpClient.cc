/**
 * @project identt
 * @file src/query/HttpClient.cc
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
 *  HttpClient.cc : Http client implementation
 *
 */

#include <query/SydentQuery.hpp> // define on top
#include <query/HttpClient.hpp>
#include <cpr/cpr.h>

/**
* constructor
*
*/
::identt::query::HttpClient::HttpClient() {}

/**
* destructor
*/
::identt::query::HttpClient::~HttpClient () {}

/**
* GetDefault : send jsonto Remote and get output
*
*/
bool ::identt::query::HttpClient::GetDefault(::identt::utils::SharedTable::pointer stptr, std::string url,
        cpr::Parameters myparams, std::string& returns, bool nothrow)
{
	try {
		auto r = cpr::Get(
		             cpr::Url{url},
		             myparams,
		cpr::Header{
			{"User-Agent", "Sydent"}
		});

		if (r.status_code != 200)
			throw identt::BadDataException("Bad HTTP Status for remote");

		returns=std::move(r.text);
	} catch (...) {
		if (nothrow) return false;
		std::rethrow_exception(std::current_exception());
	}
	return true;
}

/**
* PostJson : send jsonto Remote and get output
*
*/
bool ::identt::query::HttpClient::PostJson(::identt::utils::SharedTable::pointer stptr, std::string url,
        std::string& payload, std::string& returns, bool nothrow)
{
	try {
		auto r = cpr::Post(
		             cpr::Url{url},
		             cpr::Body{payload},
		cpr::Header{
			{"Content-Type", "application/json"},
			{"User-Agent", "Sydent"}
		});

		if (r.status_code != 200)
			throw identt::BadDataException("Bad HTTP Status for remote");

		returns=std::move(r.text);
	} catch (...) {
		if (nothrow) return false;
		std::rethrow_exception(std::current_exception());
	}
	return true;
}

