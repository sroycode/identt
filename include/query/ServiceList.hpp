/**
 * @project identt
 * @file include/query/ServiceList.hpp
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
 *  ServiceList.hpp :   Set up Service List
 *
 */
#ifndef _IDENTT_QUERY_SERVICELIST_HPP_
#define _IDENTT_QUERY_SERVICELIST_HPP_

#include "QueryBase.hpp"
#include "InfoService.hpp"
#include "ValidateService.hpp"
#include "ThreePidService.hpp"
#include "PubKeyService.hpp"
#include "LookupService.hpp"
#include "InviteService.hpp"
#include "MailSmsService.hpp"

#define IDENTT_SERVICELIST_SCOPE_HTTP \
		identt::query::InfoService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::ValidateService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::ThreePidService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::PubKeyService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::LookupService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::InviteService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::MailSmsService<HttpServerT>(sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP);

#define IDENTT_SERVICELIST_SCOPE_HTTPS

#endif // _IDENTT_QUERY_SERVICELIST_HPP_
