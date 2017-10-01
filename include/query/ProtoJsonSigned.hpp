/**
 * @project identt
 * @file include/query/ProtoJsonSigned.hpp
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
 *  ProtoJsonSigned.hpp :  Headers for Protobuf<->Json signed
 *
 */
#ifndef _IDENTT_QUERY_PROTOJSON_SIGNED_HPP_
#define _IDENTT_QUERY_PROTOJSON_SIGNED_HPP_

#include <query/ProtoJson.hpp>

namespace identt {
namespace query {

struct SignatureT {
	std::string org;
	std::string key;
	std::string value;
};

/**
* pb2signedjson : Convert protobuf to signed json simple no unsigned
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @param str
*   std::string& to string
*
* @param stack of signatures
*   std::vector<SignatureT>& signatures
*
* @return
*   none
*/
void pb2signedjson(const google::protobuf::Message* msg, std::string& str, std::vector<SignatureT>& signatures);

/**
* pb2signedjson : Convert protobuf to signed json with unsigned
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @param umsg
*   const google::protobuf::Message* from msg pointer for unsigned
*
* @param str
*   std::string& to string
*
* @param stack of signatures
*   std::vector<SignatureT>& signatures
*
* @return
*   none
*/
void pb2signedjson(const google::protobuf::Message* msg, const google::protobuf::Message* umsg,
                   std::string& str, std::vector<SignatureT>& signatures);

/**
* signedjson2unsigned : Convert signed json unsigned json string for verification
*
* @param input
*   const std::string& from string
*
* @param output
*   std::string& to string
*
* @param umsg
*   std::string unsigned part of message
*
* @param err
*   std::string& error string
*
* @param stack of signatures
*   std::vector<SignatureT>& signatures
*
* @return
*   int
*/
int signedjson2unsigned(const std::string& input, std::string& output, std::string& umsg, std::string& err, std::vector<SignatureT>& signatures);

/**
* unsigned2signedjson : Convert uniigned json to signed json
*
* @param input
*   const std::string& from string signed
*
* @param uinput
*   const std::string& from string unsigned
*
* @param output
*   std::string& to string
*
* @param err
*   std::string& error string
*
* @param stack of signatures
*   std::vector<SignatureT>& signatures
*
* @return
*   int
*/
int unsigned2signedjson(const std::string& input, const std::string& uinput,
                        std::string& output, std::string& err, std::vector<SignatureT>& signatures);

}
}

#endif // _IDENTT_QUERY_PROTOJSON_SIGNED_HPP_ 
