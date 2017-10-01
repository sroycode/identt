/**
 * @project identt
 * @file src/store/LookupService.cc
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
 *  LookupService.cc : Implementation of Lookup Service
 *
 */

#include <store/LookupService.hpp>

#include <query/bin2ascii.h>
#include <query/SignedJson.hpp> // includes QueryBase

#include <store/GlobalAssocTable.hpp>
#include <store/TokenAuthTable.hpp>

/**
* LookupAction : Service Endpoint Lookup private
*
*/
void identt::store::LookupService::LookupAction(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::LookupQueryT* query,
    ::identt::query::LookupResultT* result)
{
	// uint64_t expires = THREEPID_SESSION_VALID_LIFETIME_MS + currtime;
	// globalassoc
	::identt::store::GlobalAssocT globalassoc;
	::identt::store::GlobalAssocTable::MapT globalassoc_map;
	::identt::store::GlobalAssocTable globalassoc_table{stptr->getDB()};
	globalassoc.set_medium( query->medium() );
	globalassoc.set_address( query->address() );
	bool globalassoc_found = globalassoc_table.GetMany(
	                             &globalassoc,&globalassoc_map,
	                             ::identt::store::I_GLOBALASSOC_MEDIUM_ADDRESS,
	                             0,1
	                         );
	if (globalassoc_found && globalassoc_map.size()>0) {
		// get one anyway shreos
		auto it = globalassoc_map.begin();
		it->second.Swap(&globalassoc);
		result->set_address( globalassoc.address() );
		result->set_medium( globalassoc.medium() );
		result->set_mxid( globalassoc.mxid() );
		result->set_not_after( globalassoc.not_after() );
		result->set_not_before( globalassoc.not_before() );
		result->set_ts( globalassoc.ts() );
	}
}

/**
* LookupAction : Service Endpoint Lookup
*
*/
void identt::store::LookupService::LookupAction(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::LookupDataT* lact)
{
		this->LookupAction(stptr,lact->mutable_query(),lact->mutable_result());
}

/**
* BulkLookupAction : Service Endpoint Lookup
*
*/
void identt::store::LookupService::BulkLookupAction(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::BulkLookupDataT* blact)
{
    ::identt::query::BulkLookupQueryT* query = blact->mutable_query();
    ::identt::query::BulkLookupResultT* result = blact->mutable_result();
	// globalassobc
	for (auto i=0; i<query->threepids_size(); ++i) {
		::identt::query::LookupResultT one_result;
		this->LookupAction(stptr,query->mutable_threepids(i),&one_result);
		if (one_result.mutable_mxid()->length()>0)
			one_result.Swap(result->add_threepids());
	}
}

