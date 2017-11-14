/**
 * @project identt
 * @file include/query/WebServiceList.hpp
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
 *  WebServiceList.hpp :   Set up Service List
 *
 */
#ifndef _IDENTT_QUERY_WEBSERVICELIST_HPP_
#define _IDENTT_QUERY_WEBSERVICELIST_HPP_

#include <query/NotFoundService.hpp>
#include <query/HelpQuery.hpp>
#include <query/QueryBase.hpp>
#include <query/InfoService.hpp>
#include <query/ValidateService.hpp>
#include <query/ThreePidService.hpp>
#include <query/PubKeyService.hpp>
#include <query/LookupService.hpp>
#include <query/InviteService.hpp>
#include <query/MailSmsService.hpp>

#define IDENTT_WEBSERVICELIST_SCOPE_HTTP \
		identt::query::NotFoundService<HttpServerT> {sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP}; \
		identt::query::HelpQuery::pointer helpquery = std::make_shared<identt::query::HelpQuery>(); \
		identt::query::InfoService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::ValidateService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::ThreePidService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::PubKeyService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::LookupService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::InviteService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP); \
		identt::query::MailSmsService<HttpServerT>(sharedtable,server,helpquery,IDENTT_SERVICE_SCOPE_HTTP);

#define IDENTT_WEBSERVICELIST_SCOPE_HTTPS \
		identt::query::NotFoundService<HttpServerT> {sharedtable,server,IDENTT_SERVICE_SCOPE_HTTP};

#endif // _IDENTT_QUERY_WEBSERVICELIST_HPP_
