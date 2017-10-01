/**
 * @project identt
 * @file include/query/ProtoJson.hpp
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
 *  ProtoJson.hpp :  Headers for Protobuf<->Json implementation (modified from https://github.com/yinqiwen/pbjson )
 *
 */
#ifndef _IDENTT_QUERY_PROTOJSON_HPP_
#define _IDENTT_QUERY_PROTOJSON_HPP_

#include <string>
#include <vector>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <utils/BaseUtils.hpp>

#ifndef IDENTT_ERR_INVALID_ARG
#define IDENTT_ERR_INVALID_ARG          -1
#endif

#ifndef IDENTT_ERR_INVALID_PB
#define IDENTT_ERR_INVALID_PB           -2
#endif

#ifndef IDENTT_ERR_UNKNOWN_FIELD
#define IDENTT_ERR_UNKNOWN_FIELD        -3
#endif

#ifndef IDENTT_ERR_INVALID_JSON
#define IDENTT_ERR_INVALID_JSON         -4
#endif

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_ASSERT(x)   if(!(x)) throw identt::BadJsonException();
#include <rapidjson/document.h>


#include <query/JsonElem.hpp>

namespace identt {
namespace query {

/**
* pb2json : Convert protobuf to json
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @param str
*   std::string& to string
*
* @return
*   none
*/
void pb2json(const google::protobuf::Message* msg, std::string& str);

/**
* pb2jsonobject : Convert protobuf to json object
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @return
*   rapidjson::Value*
*/
rapidjson::Value* pb2jsonobject(const google::protobuf::Message* msg);

/**
* pb2jsonobject : Convert protobuf to json object
*
* @param msg
*   const google::protobuf::Message* from msg pointer
*
* @param allocator
*   rapidjson::Value::AllocatorType& allocator to use
*
* @return
*   rapidjson::Value*
*/
rapidjson::Value* pb2jsonobject(const google::protobuf::Message* msg, rapidjson::Value::AllocatorType& allocator);

/**
* json2string : Convert json to string
*
* @param json
*   const rapidjson::Value* input json object
*
* @param str
*   std::string& to string
*
* @return
*   none
*/
void json2string(const rapidjson::Value* json, std::string& str);

/**
* json2pb : Convert json to protobuf
*
* @param json
*   std::string& from string
*
* @param msg
*   google::protobuf::Message* to msg pointer
*
* @param err
*   std::string& error string
*
* @return
*   int
*/
int json2pb(const std::string& json, google::protobuf::Message* msg, std::string& err);

/**
* jsonobject2pb : Convert json object to protobuf
*
* @param json
*   rapidjson::Value* input json object
*
* @param msg
*   const google::protobuf::Message* to msg pointer
*
* @param err
*   std::string& error string
*
* @return
*   int
*/
int jsonobject2pb(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err);

/**
* arr2json : Generate a Json for array
*
* @param data
*   std::vector< JsonElem > data
*
* @return
*   str::string
*/
std::string arr2json(std::vector<JsonElem> data);

/**
* err2json : Generate a Json for Error Message
*
* @param errorcode
*   std::string error code string
*
* @param error
*   std::string& error string
*
* @return
*   str::string
*/
std::string err2json(const std::string errorcode, const std::string error);

}
}

#endif // _IDENTT_QUERY_PROTOJSON_HPP_ 
