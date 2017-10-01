/**
 * @project identt
 * @file src/query/ProtoForm.cc
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
 *  ProtoForm.cc :  Form to Protobuf implementation
 *
 */
#include <query/ProtoForm.hpp>

#include <query/bin2ascii.h>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <boost/lexical_cast.hpp>

inline char from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/**
* urldecode : plain url-decode
*
*/
std::unordered_map<std::string,std::string> identt::query::urldecode(const std::string& text)
{
	std::unordered_map<std::string,std::string> m;
	std::string name;
	char h;
	std::ostringstream escaped;
	escaped.str("");
	escaped.clear();
	bool name_set=false;

	for (auto i = text.begin(), n = text.end(); i != n; ++i) {
		std::string::value_type c = (*i);

		if (c == '%') {
			if (i[1] && i[2]) {
				h = from_hex(i[1]) << 4 | from_hex(i[2]);
				escaped << h;
				i += 2;
			}
		} else if (c == '+') {
			escaped << ' ';
		} else if (c == '=') {
			name = escaped.str();
			escaped.str("");
			escaped.clear();
			name_set=true;
		} else if (c == '&') {
			if (name_set)
				m[name]=escaped.str();
			escaped.str("");
			escaped.clear();
			name_set=false;
		} else {
			escaped << c;
		}
	}

	if (name_set)
		m[name]=escaped.str();
	else
		m[escaped.str()]="";
	escaped.str("");
	escaped.clear();

	return m;
}

/**
* urlencode : plain url-encode
*
*/
std::string identt::query::urlencode(const std::string &value)
{
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (auto i = value.cbegin(), n = value.cend(); i != n; ++i) {
		auto c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << std::uppercase;
		escaped << '%' << std::setw(2) << int((unsigned char) c);
		escaped << std::nouppercase;
	}

	return escaped.str();
}

/**
* form2pb : Convert url-encoded form to protobuf
*
*/
void identt::query::form2pb(const std::string& form, google::protobuf::Message* msg)
{
	std::unordered_map<std::string,std::string> fieldmap { urldecode(form) };
	const google::protobuf::Descriptor *d = msg->GetDescriptor();
	const google::protobuf::Reflection *ref = msg->GetReflection();
	if (!d || !ref) {
		throw ::identt::BadQueryException("invalid pb object");
	}
	for (auto it = fieldmap.begin() ; it!= fieldmap.end(); ++it) {
		const char* name = it->first.c_str();
		const google::protobuf::FieldDescriptor *field = d->FindFieldByName(name);
		if (!field)
			field = ref->FindKnownExtensionByName(name);
		if (!field)
			continue; // TODO: what else
		if (field->is_repeated())
			throw ::identt::BadQueryException("pb object repeated field not supported in form");

		// now set the value
		switch (field->cpp_type()) {
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
			ref->SetInt32(msg, field, boost::lexical_cast<::google::protobuf::int32>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
			ref->SetUInt32(msg, field, boost::lexical_cast<::google::protobuf::uint32>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
			ref->SetInt64(msg, field, boost::lexical_cast<::google::protobuf::int64>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
			ref->SetUInt64(msg, field, boost::lexical_cast<::google::protobuf::uint64>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
			ref->SetDouble(msg, field, boost::lexical_cast<double>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
			ref->SetFloat(msg, field, boost::lexical_cast<float>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
			ref->SetBool(msg, field, boost::lexical_cast<bool>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
			ref->SetString(msg, field, it->second);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
			ref->SetEnumValue(msg, field, boost::lexical_cast<::google::protobuf::int32>( it->second ) );
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
			throw ::identt::BadQueryException("pb object message field not supported in form");
			break;
		}
		default:
			break;
		}
	}
}
