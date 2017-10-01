/**
 * @project identt
 * @file src/query/ProtoJsonSigned.cc
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
 *  ProtoJsonSigned.cc :  Protobuf to Json implementation (modified from https://github.com/yinqiwen/pbjson )
 *
 */
#include <query/ProtoJsonSigned.hpp>
#include <query/bin2ascii.h>

#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"

#include <sstream>


#define RETURN_ERR(id, cause)  do{ err = cause; return id;  }while(0)

rapidjson::Value* parse_msg(const google::protobuf::Message *msg, rapidjson::Value::AllocatorType& allocator);
rapidjson::Value* field2json(const google::protobuf::Message *msg, const google::protobuf::FieldDescriptor *field,
                             rapidjson::Value::AllocatorType& allocator);
int parse_json(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err);
int parse_json(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err);
int json2field(const rapidjson::Value* json, google::protobuf::Message* msg,
               const google::protobuf::FieldDescriptor *field, std::string& err);

/**
* pb2signedjson : Convert protobuf to signed json simple no unsigned
*
*/
void identt::query::pb2signedjson(const google::protobuf::Message* msg,
                                  std::string& str,
                                  std::vector<SignatureT>& signatures)
{
	rapidjson::Value::AllocatorType allocator;
	rapidjson::Value* json = parse_msg(msg, allocator);
	rapidjson::Pointer("/signatures").Create(*json,allocator);
	rapidjson::Value* sign = rapidjson::Pointer("/signatures").Get(*json);
	if (!sign->IsObject()) sign->SetObject();
	for (auto& s : signatures) {
		rapidjson::Value entry(rapidjson::kObjectType);
		entry.AddMember( rapidjson::StringRef( s.key.c_str() ), rapidjson::StringRef( s.value.c_str() ), allocator);
		sign->AddMember( rapidjson::StringRef( s.org.c_str() ),  entry , allocator);
	}
	identt::query::json2string(json, str);
	delete json;
}

/**
* pb2signedjson : Convert protobuf to signed json with unsigned
*
*/
void identt::query::pb2signedjson(const google::protobuf::Message* msg,
                                  const google::protobuf::Message* umsg,
                                  std::string& str, std::vector<SignatureT>& signatures)
{
	rapidjson::Value::AllocatorType allocator;
	rapidjson::Value* json = parse_msg(msg, allocator);
	rapidjson::Pointer("/signatures").Create(*json,allocator);
	rapidjson::Value* sign = rapidjson::Pointer("/signatures").Get(*json);
	if (!sign->IsObject()) sign->SetObject();
	for (auto& s : signatures) {
		rapidjson::Value entry(rapidjson::kObjectType);
		entry.AddMember( rapidjson::StringRef( s.key.c_str() ), rapidjson::StringRef( s.value.c_str() ), allocator);
		sign->AddMember( rapidjson::StringRef( s.org.c_str() ),  entry , allocator);
	}
	// add unsigned
	rapidjson::Pointer("/unsigned").Create(*json,allocator);
	rapidjson::Value* unsign = rapidjson::Pointer("/unsigned").Get(*json);
	unsign = parse_msg(umsg, allocator);

	// convert to string
	identt::query::json2string(json, str);
	delete json;
}


/**
* signedjson2unsigned : Convert signed json unsigned json string for verification
*
*/
int identt::query::signedjson2unsigned(const std::string& input,
                                       std::string& output, std::string& umsg,
                                       std::string& err, std::vector<SignatureT>& signatures)
{
	rapidjson::Document d;
	d.Parse<0>(input.c_str());
	if (d.HasParseError()) {
		err += d.GetParseError();
		return IDENTT_ERR_INVALID_ARG;
	}
	if (rapidjson::Value* sign = rapidjson::Pointer("/signatures").Get(d)) {
		for (auto it = sign->MemberBegin() ; it!= sign->MemberEnd() ; ++it) {
			identt::query::SignatureT s;
			s.org = it->name.GetString();
			if (!it->value.IsObject() ) return IDENTT_ERR_INVALID_ARG;
			for (auto jt = it->value.MemberBegin() ; jt!= it->value.MemberEnd() ; ++jt) {
				s.key = jt->name.GetString();
				s.value = jt->value.GetString();
				signatures.emplace_back(s);
			}
		}
		bool ok = rapidjson::EraseValueByPointer(d, "/signatures");
	}
	if (rapidjson::Value* unsign = rapidjson::Pointer("/unsigned").Get(d)) {
		json2string(unsign, umsg);
		bool ok = rapidjson::EraseValueByPointer(d, "/unsigned");
	}
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	output.clear();
	output.append(buffer.GetString(), buffer.GetSize());
	return 0;
}

/**
* unsigned2signedjson : Convert signed json unsigned json string for verification
*
*/
int identt::query::unsigned2signedjson(const std::string& input, const std::string& uinput,
                                       std::string& output, std::string& err, std::vector<SignatureT>& signatures)
{
	rapidjson::Document d;
	d.Parse<0>(input.c_str());
	if (d.HasParseError()) {
		err += d.GetParseError();
		return IDENTT_ERR_INVALID_ARG;
	}
	rapidjson::Pointer("/signatures").Create(d,d.GetAllocator() );
	rapidjson::Value* sign = rapidjson::Pointer("/signatures").Get(d);
	if (!sign->IsObject()) sign->SetObject();
	for (auto& s : signatures) {
		rapidjson::Value entry(rapidjson::kObjectType);
		entry.AddMember( rapidjson::StringRef( s.key.c_str() ), rapidjson::StringRef( s.value.c_str() ), d.GetAllocator() );
		sign->AddMember( rapidjson::StringRef( s.org.c_str() ),  entry , d.GetAllocator() );
	}
	if (uinput.length()>2) {
		rapidjson::Pointer("/unsigned").Create(d,d.GetAllocator() );
		rapidjson::Value* unsign = rapidjson::Pointer("/unsigned").Get(d);
		// check shreos
		rapidjson::Document e;
		e.Parse(uinput.c_str());
		for (auto it = e.MemberBegin() ; it!=e.MemberEnd() ; ++it) {
			unsign->AddMember(it->name,it->value,d.GetAllocator() );
		}
	}
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	output.clear();
	output.append(buffer.GetString(), buffer.GetSize());
	return 0;
}
