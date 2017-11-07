/**
 * @project identt
 * @file src/store/LogReadService.cc
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
 *  LogReadService.cc : Implementation of LogRead Service
 *
 */

#include <store/LogReadService.hpp>
#include <store/StoreTrans.hpp>

/**
* LogReadAction : Service Endpoint LogRead
*
*/
void identt::store::LogReadService::LogReadAction(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::store::TransListT* translist)
{
	// ::identt::store::TransT translist;
	::identt::store::TransListTable translist_table{stptr->maindb.Get()};
	bool translist_found = translist_table.GetOne(
	                           &translist,&translist_map,
	                           ::identt::store::I_GLOBALASSOC_MEDIUM_ADDRESS,
	                           0,1
	                       );
}

/**
* LogReadAction : Service Endpoint LogRead private
*
*/
void identt::store::LogReadService::LogReadAction(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::LogReadQueryT* query,
    ::identt::query::LogReadResultT* result)
{
	// uint64_t expires = THREEPID_SESSION_VALID_LIFETIME_MS + currtime;
	// translist
	::identt::store::TransListTable::MapT translist_map;
	translist.set_medium( query->medium() );
	translist.set_address( query->address() );
	if (translist_found && translist_map.size()>0) {
		// get one anyway shreos
		auto it = translist_map.begin();
		it->second.Swap(&translist);
		result->set_address( translist.address() );
		result->set_medium( translist.medium() );
		result->set_mxid( translist.mxid() );
		result->set_not_after( translist.not_after() );
		result->set_not_before( translist.not_before() );
		result->set_ts( translist.ts() );
	}
}

