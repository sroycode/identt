/**
 * @project identt
 * @file include/query/QueryBase.hpp
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
 *  QueryBase.hpp :  query base includes
 *
 */
#ifndef _IDENTT_QUERY_QUERYBASE_HPP_
#define _IDENTT_QUERY_QUERYBASE_HPP_

#include <utils/BaseUtils.hpp>
#include <query/SydentQuery.hpp> // define on top
#include <async++.h>
#include <functional>
#include <boost/algorithm/string.hpp>
#include <http/ServiceBase.hpp>
#include "../proto/Query.pb.h"
#include "../proto/Store.pb.h"
#include "ProtoForm.hpp"
#include "ProtoJson.hpp"


namespace identt {
namespace query {

/**
* GetTime : get time milliseconds
*
* @return
*   uint64_t time
*/
inline uint64_t GetTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
* SimpleHash : To generate sid
*
* @param str
*   std::string& string to change
*
* @return
*   std::string hash
*/
static inline std::string SimpleHash (std::string str)
{
	boost::algorithm::to_upper(str);
	uint64_t t=0;
	size_t counter=8;
	for (char& i : str) {
		t=t<<1L;
		t+= i;
		if (--counter==0) break;
	}
	return std::to_string(t);
}

} // namspace query
} // namspace identt

#endif /* _IDENTT_QUERY_QUERYBASE_HPP_ */
