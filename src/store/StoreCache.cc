/**
 * @project identt
 * @file src/store/StoreCache.cc
 * @author  S Roychowdhury <sroycode AT gmail DOT com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018 S Roychowdhury.
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
 *  StoreCache.cc : cache store
 *
 */

#include "store/StoreCache.hpp"

/**
* Constructor : default private
*
*/
identt::store::StoreCache::StoreCache () {}

/**
* destructor
*/
identt::store::StoreCache::~StoreCache () {}

/**
* SetAssoc : add an assoc entry
*
*/
void identt::store::StoreCache::SetAssoc(std::string hash, identt::store::StoreCache::AssocT assoc)
{
	WriteLockT writelock(mutex_);
	DLOG(INFO) << "Added Assoc Cache: " << hash;
	assoc_map[hash] = assoc;
}

/**
* GetAssoc : get assoc
*
*/
bool identt::store::StoreCache::GetAssoc(std::string hash, identt::store::StoreCache::AssocT& assoc)
{
	ReadLockT readlock(mutex_);
	auto it = assoc_map.find(hash);
	if (it != assoc_map.end()) {
		assoc = it->second;
		return true;
	}
	return false;
}

/**
* DelAssoc : del assoc entry
*
*/
void identt::store::StoreCache::DelAssoc(std::string hash)
{
	WriteLockT writelock(mutex_);
	DLOG(INFO) << "Delete Assoc Cache: " << hash;
	auto it = assoc_map.find(hash);
	if (it != assoc_map.end())
		assoc_map.erase(it);
}

/**
* AddToCache : adds a transaction to cache
*
*/
void identt::store::StoreCache::AddToCache(::identt::store::TransactionT* trans)
{
	if (trans->item_size()==0) return;
	for (auto i=0; i<trans->item_size(); ++i) {
		std::string key = trans->mutable_item(i)->key();
		if (key.length() != ( IDENTT_KEYID_LEN + IDENTT_NODE_LEN )) continue;
		auto kp = DecodePrimaryKey( key );
		if ( kp.first == K_LOCALASSOC ) {
			::identt::store::LocalAssocT record;
			if (!record.ParseFromString(trans->mutable_item(i)->value())) continue;
			std::string medium_address = EncodeSecondaryKey<std::string,std::string>(
			                                 U_LOCALASSOC_MEDIUM_ADDRESS, record.medium(),record.address() );
			if ( trans->mutable_item(i)->to_del() ) DelAssoc( medium_address );
			else SetAssoc(medium_address, trans->mutable_item(i)->value() );
		}
	}
}
