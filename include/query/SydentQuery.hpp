/**
 * @project identt
 * @file include/query/SydentQuery.hpp
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
 *  SydentQuery.hpp :  sydent specific query base includes
 *
 */
#ifndef _IDENTT_QUERY_SYDENTQUERY_HPP_
#define _IDENTT_QUERY_SYDENTQUERY_HPP_

#include <utils/BaseUtils.hpp>
#include <map>

#define THREEPID_DEFAULT_ALGO "ed25519"
#define THREEPID_DEFAULT_ALGO_ID "0"
#define THREEPID_DEFAULT_ALGO_WITH_ID THREEPID_DEFAULT_ALGO ":" THREEPID_DEFAULT_ALGO_ID
#define THREEPID_EXTMAIL_ALGO "ed25519"
#define THREEPID_EXTMAIL_ALGO_ID "mail"
#define THREEPID_EXTMAIL_ALGO_WITH_ID THREEPID_EXTMAIL_ALGO ":" THREEPID_EXTMAIL_ALGO_ID

#define THREEPID_SESSION_VALID_LIFETIME_MS 24 * 60 * 60 * 1000

#define IDENTT_SYDENT_ERROR_MIN         101
#define M_BAD_JSON                      101
#define M_DESTINATION_REJECTED          102
#define M_EMAIL_SEND_ERROR              103
#define M_INCORRECT_CLIENT_SECRET       104
#define M_INVALID_EMAIL                 105
#define M_INVALID_PARAM                 106
#define M_INVALID_PHONE_NUMBER          107
#define M_MISSING_PARAMS                108
#define M_NOT_JSON                      109
#define M_NO_VALID_SESSION              110
#define M_SESSION_EXPIRED               111
#define M_SESSION_NOT_VALIDATED         112
#define M_UNKNOWN                       113
#define M_UNKNOWN_PEER                  114
#define M_VERIFICATION_FAILED           115
#define M_THREEPID_IN_USE               116
#define IDENTT_SYDENT_ERROR_MAX         116

namespace identt {
namespace query {

const static std::map<int, std::string> SydentErrors = {
	{ M_BAD_JSON,                         "M_BAD_JSON"},
	{ M_DESTINATION_REJECTED,             "M_DESTINATION_REJECTED"},
	{ M_EMAIL_SEND_ERROR,                 "M_EMAIL_SEND_ERROR"},
	{ M_INCORRECT_CLIENT_SECRET,          "M_INCORRECT_CLIENT_SECRET"},
	{ M_INVALID_EMAIL,                    "M_INVALID_EMAIL"},
	{ M_INVALID_PARAM,                    "M_INVALID_PARAM"},
	{ M_INVALID_PHONE_NUMBER,             "M_INVALID_PHONE_NUMBER"},
	{ M_MISSING_PARAMS,                   "M_MISSING_PARAMS"},
	{ M_NOT_JSON,                         "M_NOT_JSON"},
	{ M_NO_VALID_SESSION,                 "M_NO_VALID_SESSION"},
	{ M_SESSION_EXPIRED,                  "M_SESSION_EXPIRED"},
	{ M_SESSION_NOT_VALIDATED,            "M_SESSION_NOT_VALIDATED"},
	{ M_UNKNOWN,                          "M_UNKNOWN"},
	{ M_UNKNOWN_PEER,                     "M_UNKNOWN_PEER"},
	{ M_VERIFICATION_FAILED,              "M_VERIFICATION_FAILED"},
	{ M_THREEPID_IN_USE,                  "THREEPID_IN_USE"}
};

/** Valid Isd */
const static std::map<std::string,std::string> ValidIsdCodes = {
	{ "IN", "91" },
	{ "US", "1" },
	{ "GB", "44" }
};

/** Sydent Exceptions */
class SydentException : public JdException {
	using JdException::JdException;
};

} // namspace query
} // namspace identt

#endif /* _IDENTT_QUERY_SYDENTQUERY_HPP_ */
