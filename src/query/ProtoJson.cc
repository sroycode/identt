/**
 * @project identt
 * @file src/query/ProtoJson.cc
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
 *  ProtoJson.cc :  Protobuf to Json implementation (modified from https://github.com/yinqiwen/pbjson )
 *
 */
#include <query/ProtoJson.hpp>
#include <query/bin2ascii.h>

#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <sstream>


#define RETURN_ERR(id, cause)  do{ err = cause; return id;  }while(0)

rapidjson::Value* parse_msg(const google::protobuf::Message *msg, rapidjson::Value::AllocatorType& allocator);
rapidjson::Value* field2json(const google::protobuf::Message *msg, const google::protobuf::FieldDescriptor *field,
                             rapidjson::Value::AllocatorType& allocator)
{
	const google::protobuf::Reflection *ref = msg->GetReflection();
	const bool repeated = field->is_repeated();

	size_t array_size = 0;
	if (repeated) {
		array_size = ref->FieldSize(*msg, field);
	}
	rapidjson::Value* json = NULL;
	if (repeated) {
		json = new rapidjson::Value(rapidjson::kArrayType);
	}
	switch (field->cpp_type()) {
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				double value = ref->GetRepeatedDouble(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(ref->GetDouble(*msg, field));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				float value = ref->GetRepeatedFloat(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(ref->GetFloat(*msg, field));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				int64_t value = ref->GetRepeatedInt64(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(static_cast<int64_t>(ref->GetInt64(*msg, field)));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				uint64_t value = ref->GetRepeatedUInt64(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(static_cast<uint64_t>(ref->GetUInt64(*msg, field)));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				int32_t value = ref->GetRepeatedInt32(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(ref->GetInt32(*msg, field));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				uint32_t value = ref->GetRepeatedUInt32(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(ref->GetUInt32(*msg, field));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				bool value = ref->GetRepeatedBool(*msg, field, i);
				rapidjson::Value v(value);
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(ref->GetBool(*msg, field));
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
		bool is_binary = field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES;
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				std::string value = ref->GetRepeatedString(*msg, field, i);
				if (is_binary) {
					value = b64_encode(value);
				}
				rapidjson::Value v(value.c_str(), static_cast<rapidjson::SizeType>(value.size()), allocator);
				json->PushBack(v, allocator);
			}
		} else {
			std::string value = ref->GetString(*msg, field);
			if (is_binary) {
				value = b64_encode(value);
			}
			json = new rapidjson::Value(value.c_str(), value.size(), allocator);
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				const google::protobuf::Message *value = &(ref->GetRepeatedMessage(*msg, field, i));
				rapidjson::Value* v = parse_msg(value, allocator);
				json->PushBack(*v, allocator);
				delete v;
			}
		} else {
			const google::protobuf::Message *value = &(ref->GetMessage(*msg, field));
			json = parse_msg(value, allocator);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		if (repeated) {
			for (size_t i = 0; i != array_size; ++i) {
				const google::protobuf::EnumValueDescriptor* value = ref->GetRepeatedEnum(*msg, field, i);
				rapidjson::Value v(value->number());
				json->PushBack(v, allocator);
			}
		} else {
			json = new rapidjson::Value(ref->GetEnum(*msg, field)->number());
		}
		break;
	default:
		break;
	}
	return json;
}

rapidjson::Value* parse_msg(const google::protobuf::Message *msg, rapidjson::Value::AllocatorType& allocator)
{
	const google::protobuf::Descriptor *d = msg->GetDescriptor();
	if (!d)
		return NULL;
	size_t count = d->field_count();
	rapidjson::Value* root = new rapidjson::Value(rapidjson::kObjectType);
	if (!root)
		return NULL;
	for (size_t i = 0; i != count; ++i) {
		const google::protobuf::FieldDescriptor *field = d->field(i);
		if (!field) {
			delete root;
			return NULL;
		}

		const google::protobuf::Reflection *ref = msg->GetReflection();
		if (!ref) {
			delete root;
			return NULL;
		}
		if (field->is_optional() && !ref->HasField(*msg, field)) {
			//do nothing
		} else {
			rapidjson::Value* field_json = field2json(msg, field, allocator);
			rapidjson::Value field_name(field->name().c_str(), field->name().size());
			root->AddMember(field_name, *field_json, allocator);
			delete field_json;
		}
	}
	return root;
}
int parse_json(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err);
int json2field(const rapidjson::Value* json, google::protobuf::Message* msg,
               const google::protobuf::FieldDescriptor *field, std::string& err)
{
	const google::protobuf::Reflection *ref = msg->GetReflection();
	const bool repeated = field->is_repeated();
	switch (field->cpp_type()) {
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
		if (json->GetType() != rapidjson::kNumberType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a number");
		}
		if (repeated) {
			ref->AddInt32(msg, field, (int32_t) json->GetInt());
		} else {
			ref->SetInt32(msg, field, (int32_t) json->GetInt());
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
		if (json->GetType() != rapidjson::kNumberType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a number");
		}
		if (repeated) {
			ref->AddUInt32(msg, field, json->GetUint());
		} else {
			ref->SetUInt32(msg, field, json->GetUint());
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
		if (json->GetType() != rapidjson::kNumberType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a number");
		}
		if (repeated) {
			ref->AddInt64(msg, field, json->GetInt64());
		} else {
			ref->SetInt64(msg, field, json->GetInt64());
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
		if (json->GetType() != rapidjson::kNumberType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a number");
		}
		if (repeated) {
			ref->AddUInt64(msg, field, json->GetUint64());
		} else {
			ref->SetUInt64(msg, field, json->GetUint64());
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
		if (json->GetType() != rapidjson::kNumberType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a number");
		}
		if (repeated) {
			ref->AddDouble(msg, field, json->GetDouble());
		} else {
			ref->SetDouble(msg, field, json->GetDouble());
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
		if (json->GetType() != rapidjson::kNumberType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a number");
		}
		if (repeated) {
			ref->AddFloat(msg, field, json->GetDouble());
		} else {
			ref->SetFloat(msg, field, json->GetDouble());
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
		if (json->GetType() != rapidjson::kTrueType && json->GetType() != rapidjson::kFalseType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a bool");
		}
		bool v = json->GetBool();
		if (repeated) {
			ref->AddBool(msg, field, v);
		} else {
			ref->SetBool(msg, field, v);
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
		if (json->GetType() != rapidjson::kStringType) {
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not a string");
		}
		const char* value = json->GetString();
		uint32_t str_size = json->GetStringLength();
		std::string str_value(value, str_size);
		if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES) {
			if (repeated) {
				ref->AddString(msg, field, b64_decode(str_value));
			} else {
				ref->SetString(msg, field, b64_decode(str_value));
			}
		} else {
			if (repeated) {
				ref->AddString(msg, field, str_value);
			} else {
				ref->SetString(msg, field, str_value);
			}
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
		google::protobuf::Message *mf = (repeated) ? ref->AddMessage(msg, field) : ref->MutableMessage(msg, field);
		return parse_json(json, mf, err);
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
		const google::protobuf::EnumDescriptor *ed = field->enum_type();
		const google::protobuf::EnumValueDescriptor *ev = 0;
		if (json->GetType() == rapidjson::kNumberType) {
			ev = ed->FindValueByNumber(json->GetInt());
		} else if (json->GetType() == rapidjson::kStringType) {
			ev = ed->FindValueByName(json->GetString());
		} else
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not an integer or string");
		if (!ev)
			RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Enum value not found");
		if (repeated) {
			ref->AddEnum(msg, field, ev);
		} else {
			ref->SetEnum(msg, field, ev);
		}
		break;
	}
	default:
		break;
	}
	return 0;
}

int parse_json(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err)
{
	if (NULL == json || json->GetType() != rapidjson::kObjectType) {
		return IDENTT_ERR_INVALID_ARG;
	}
	const google::protobuf::Descriptor *d = msg->GetDescriptor();
	const google::protobuf::Reflection *ref = msg->GetReflection();
	if (!d || !ref) {
		RETURN_ERR(IDENTT_ERR_INVALID_PB, "invalid pb object");
	}
	for (rapidjson::Value::ConstMemberIterator itr = json->MemberBegin(); itr != json->MemberEnd(); ++itr) {
		const char* name = itr->name.GetString();
		const google::protobuf::FieldDescriptor *field = d->FindFieldByName(name);
		if (!field)
			field = ref->FindKnownExtensionByName(name);
		if (!field)
			continue; // TODO: we should not fail here, instead write this value into an unknown field
		if (itr->value.GetType() == rapidjson::kNullType) {
			ref->ClearField(msg, field);
			continue;
		}
		if (field->is_repeated()) {
			if (itr->value.GetType() != rapidjson::kArrayType)
				RETURN_ERR(IDENTT_ERR_INVALID_JSON, "Not array");
			for (rapidjson::Value::ConstValueIterator ait = itr->value.Begin(); ait != itr->value.End(); ++ait) {
				int ret = json2field(ait, msg, field, err);
				if (ret != 0) {
					return ret;
				}
			}
		} else {
			int ret = json2field(&(itr->value), msg, field, err);
			if (ret != 0) {
				return ret;
			}
		}
	}
	return 0;
}

/**
* json2string : Convert json to string
*
*/
void identt::query::json2string(const rapidjson::Value* json, std::string& str)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json->Accept(writer);
	str.append(buffer.GetString(), buffer.GetSize());
}

/**
* pb2json : Convert protobuf to json
*
*/
void identt::query::pb2json(const google::protobuf::Message* msg, std::string& str)
{
	rapidjson::Value::AllocatorType allocator;
	rapidjson::Value* json = parse_msg(msg, allocator);
	identt::query::json2string(json, str);
	delete json;
}

/**
* pb2jsonobject : Convert protobuf to json object
*
*/
rapidjson::Value* identt::query::pb2jsonobject(const google::protobuf::Message* msg)
{
	rapidjson::Value::AllocatorType allocator;
	return parse_msg(msg, allocator);
}

/**
* pb2jsonobject : Convert protobuf to json object
*
*/
rapidjson::Value* identt::query::pb2jsonobject(const google::protobuf::Message* msg, rapidjson::Value::AllocatorType& allocator)
{
	return parse_msg(msg, allocator);
}

/**
* json2pb : Convert json to protobuf
*
*/
int identt::query::json2pb(const std::string& json, google::protobuf::Message* msg, std::string& err)
{
	rapidjson::Document d;
	d.Parse<0>(json.c_str());
	if (d.HasParseError()) {
		err += d.GetParseError();
		return IDENTT_ERR_INVALID_ARG;
	}
	int ret = identt::query::jsonobject2pb(&d, msg, err);
	return ret;
}

/**
* jsonobject2pb : Convert json object to protobuf
*
*/
int identt::query::jsonobject2pb(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err)
{
	return parse_json(json, msg, err);
}

/**
* arr2json : Generate a Json for array
*
*/
std::string identt::query::arr2json(std::vector< identt::query::JsonElem > data)
{
	rapidjson::Document d;
	d.SetObject();
	for (auto& i : data) {
		rapidjson::Value name(rapidjson::StringRef(i.nam));
		rapidjson::Value val;
		switch (i.typ) {
		default:
		case identt::query::JsonElem::JsonElemE::IDENTT_JE_STRING: {
			val.SetString(rapidjson::StringRef(i.val));
			break;
		}
		case identt::query::JsonElem::JsonElemE::IDENTT_JE_DOUBLE: {
			val.SetDouble(std::stod(i.val));
			break;
		}
		case identt::query::JsonElem::JsonElemE::IDENTT_JE_INT64: {
			val.SetInt64(std::stoll(i.val));
			break;
		}
		case identt::query::JsonElem::JsonElemE::IDENTT_JE_BOOL: {
			val.SetBool((bool)std::stoi(i.val));
			break;
		}
		}
		d.AddMember(name, val, d.GetAllocator());
	}
	std::string str;
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	str.append(buffer.GetString(), buffer.GetSize());
	return str;
}

/**
* err2json : Generate a Json for Error Message
*
*/
std::string identt::query::err2json(const std::string errorcode,const std::string error)
{
	return ::identt::query::arr2json({
		{ "errorcode" , errorcode },
		{ "error" , error }
	});
}

