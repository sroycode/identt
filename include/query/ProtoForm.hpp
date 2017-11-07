/**
 * @project identt
 * @file include/query/ProtoForm.hpp
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
 *  ProtoForm.hpp :  Headers for Form -> Protobuf implementation
 *
 */
#ifndef _IDENTT_QUERY_PROTOFORM_HPP_
#define _IDENTT_QUERY_PROTOFORM_HPP_

#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <utils/BaseUtils.hpp>
#include <unordered_map>

#ifndef IDENTT_ERR_INVALID_ARG
#define IDENTT_ERR_INVALID_ARG          -1
#endif

#ifndef IDENTT_ERR_INVALID_PB
#define IDENTT_ERR_INVALID_PB           -2
#endif

#ifndef IDENTT_ERR_UNKNOWN_FIELD
#define IDENTT_ERR_UNKNOWN_FIELD        -3
#endif

#ifndef IDENTT_ERR_INVALID_FORM
#define IDENTT_ERR_INVALID_FORM         -5
#endif

namespace identt {
namespace query {

/**
* urlencode : plain url-encode
*
* @param input by address
*   const std::string&
*
* @return
*   std::unordered_map<std::string,std::string>
*/
std::unordered_map<std::string,std::string> urldecode(const std::string& text);

/**
* urldecode : plain url-decode
*
* @param value
*   const std::string& input by address
*
* @return
*   std::string
*/
std::string urlencode(const std::string& value);

/**
* form2pb : Convert url-encoded form to protobuf
*
* @param form
*   const std::string& from encoded string
*
* @param msg
*   google::protobuf::Message* to protobuf
*
* @return
*   none
*/
void form2pb(const std::string& form, google::protobuf::Message* msg);

}
}

#endif // _IDENTT_QUERY_PROTOFORM_HPP_ 
