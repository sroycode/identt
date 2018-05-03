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

#include <async++.h>

#include <store/LookupService.hpp>

#include <query/bin2ascii.h>

#include <store/GlobalAssocTable.hpp>
#include <store/LocalAssocTable.hpp>

/**
* LookupAction : Service Endpoint Lookup
*
*/
void identt::store::LookupService::LookupAction(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::LookupDataT* lact)
{
	bool lookup_uses_local = stptr->lookup_uses_local.Get();
	if (lookup_uses_local)
		this->LookupLocal(stptr,lact->mutable_query(),lact->mutable_result());
	else
		this->LookupGlobal(stptr,lact->mutable_query(),lact->mutable_result());
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

	bool lookup_uses_local = stptr->lookup_uses_local.Get();

	// globalassoc
	size_t s_size = query->threepids_size();
	if (lookup_uses_local)
		this->BulkLookupLocal(stptr,blact,0,s_size-1);
	else
		this->BulkLookupGlobal(stptr,blact,0,s_size-1);
}

/**
* LookupGlobal : Service Endpoint Lookup private
*
*/
void identt::store::LookupService::LookupGlobal(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::LookupQueryT* query,
    ::identt::query::LookupResultT* result)
{
	// uint64_t expires = THREEPID_SESSION_VALID_LIFETIME_MS + currtime;
	// globalassoc
	::identt::store::GlobalAssocT globalassoc;
	::identt::store::GlobalAssocTable::MapT globalassoc_map;
	::identt::store::GlobalAssocTable globalassoc_table{stptr->maindb.Get()};
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
* BulkLookupGlobal : Bulk Lookup Implementation
*
*/
void identt::store::LookupService::BulkLookupGlobal(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::BulkLookupDataT* blact, size_t start, size_t end)
{
	// globalassoc
	::identt::store::GlobalAssocTable globalassoc_table{stptr->maindb.Get()};

	::identt::store::GlobalAssocT globalassoc;
	::identt::store::GlobalAssocTable::MapT globalassoc_map;

	::identt::query::BulkLookupQueryT* query = blact->mutable_query();
	::identt::query::BulkLookupResultT* result = blact->mutable_result();

	size_t total = query->threepids_size();

	if (start >= total || end >= total || start > end ) return;

	for (auto i=start; i<=end; ++i) {
		globalassoc.Clear();
		globalassoc.set_medium( query->mutable_threepids(i)->medium() );
		globalassoc.set_address( query->mutable_threepids(i)->address() );
		bool globalassoc_found = globalassoc_table.GetMany(
		                             &globalassoc,&globalassoc_map,
		                             ::identt::store::I_GLOBALASSOC_MEDIUM_ADDRESS,
		                             0,1
		                         );
	}

	for (auto it= globalassoc_map.begin(); it!=globalassoc_map.end(); ++it) {
		it->second.Swap(&globalassoc);
		::identt::query::LookupResultT one_result;
		one_result.set_address( globalassoc.address() );
		one_result.set_medium( globalassoc.medium() );
		one_result.set_mxid( globalassoc.mxid() );
		one_result.set_not_after( globalassoc.not_after() );
		one_result.set_not_before( globalassoc.not_before() );
		one_result.set_ts( globalassoc.ts() );
		if (one_result.mutable_mxid()->length()>0) {
			std::lock_guard<std::mutex> lock(read_mutex);
			one_result.Swap(result->add_threepids());
		}
	}
}

/**
* LookupLocal : Lookup from LocalAssoc private
*
*/
void identt::store::LookupService::LookupLocal(
    ::identt::utils::SharedTable::pointer stptr,
    ::identt::query::LookupQueryT* query,
    ::identt::query::LookupResultT* result)
{
	// uint64_t expires = THREEPID_SESSION_VALID_LIFETIME_MS + currtime;
	// localassoc
	std::string data;
	std::string medium_address = stptr->dbcache->EncodeSecondaryKey<std::string,std::string>(
	                                 ::identt::store::U_LOCALASSOC_MEDIUM_ADDRESS,
	                                 query->medium(),query->address() );

	bool stat = stptr->dbcache->GetAssoc(medium_address,data);
	if (!stat) return;
	::identt::store::LocalAssocT localassoc;
	stat = localassoc.ParseFromString(data);
	if (!stat) return;

	result->set_address( localassoc.address() );
	result->set_medium( localassoc.medium() );
	result->set_mxid( localassoc.mxid() );
	result->set_not_after( localassoc.not_after() );
	result->set_not_before( localassoc.not_before() );
	result->set_ts( localassoc.ts() );
}

/**
* BulkLookupLocal : Bulk Lookup Implementation with LocalAssoc
*
*/
void identt::store::LookupService::BulkLookupLocal(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::BulkLookupDataT* blact, size_t start, size_t end)
{
	// localassoc
	::identt::store::LocalAssocTable localassoc_table{stptr->maindb.Get()};

	::identt::store::LocalAssocT localassoc;
	::identt::store::LocalAssocTable::MapT localassoc_map;

	::identt::query::BulkLookupQueryT* query = blact->mutable_query();
	::identt::query::BulkLookupResultT* result = blact->mutable_result();

	size_t total = query->threepids_size();

	if (start >= total || end >= total || start > end ) return;

	for (auto i=start; i<=end; ++i) {
		std::string data;
		std::string medium_address = stptr->dbcache->EncodeSecondaryKey<std::string,std::string>(
		                                 ::identt::store::U_LOCALASSOC_MEDIUM_ADDRESS,
		                                 query->mutable_threepids(i)->medium(),
		                                 query->mutable_threepids(i)->address() );
		if ( ! stptr->dbcache->GetAssoc(medium_address,data) ) continue;
		localassoc.Clear();
		if ( localassoc.ParseFromString(data) ) {
			localassoc_map[localassoc.id()]=localassoc;
		}
	}

	for (auto it= localassoc_map.begin(); it!=localassoc_map.end(); ++it) {
		it->second.Swap(&localassoc);
		::identt::query::LookupResultT one_result;
		one_result.set_address( localassoc.address() );
		one_result.set_medium( localassoc.medium() );
		one_result.set_mxid( localassoc.mxid() );
		one_result.set_not_after( localassoc.not_after() );
		one_result.set_not_before( localassoc.not_before() );
		one_result.set_ts( localassoc.ts() );
		if (one_result.mutable_mxid()->length()>0) {
			std::lock_guard<std::mutex> lock(read_mutex);
			one_result.Swap(result->add_threepids());
		}
	}
}

