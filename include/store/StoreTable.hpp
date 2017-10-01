/**
 * @project identt
 * @file include/store/StoreTable.hpp
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
 *  StoreTable.hpp :   Headers for StoreTable store as table
 *
 */
#ifndef _IDENTT_STORE_STORE_TABLE_HPP_
#define _IDENTT_STORE_STORE_TABLE_HPP_

#include <set>
#include <google/protobuf/message.h>
#include <google/protobuf/map.h>
#include "StoreLevel.hpp"

namespace identt {
namespace store {
template <class T>
class StoreTable : public StoreLevel {
public:
	using StoreLevel::dbpointer;
	using KeySetT=std::set<KeyTypeE>;
	using MapT=google::protobuf::Map<uint64_t,T>;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	* @param pkey
	*   const KeyTypeE primary key
	*
	* @param pname
	*   const std::string primary key name
	*
	* @param ukeys
	*   const KeySetT unique keys to initialize
	*
	* @param ikeys
	*   const KeySetT non unique keys to initialize
	*
	*/
	StoreTable(dbpointer trydb, const KeyTypeE pkey,const std::string pname, const KeySetT ukeys, const KeySetT ikeys)
		:
		StoreLevel(trydb),
		PrimaryKey(pkey),
		PrimaryName(pname),
		unique_keys(ukeys),
		index_keys(ikeys)
	{}

	/**
	* Destructor
	*
	*/
	virtual ~StoreTable() {}

	/**
	* make noncopyable and remove default
	*/

	StoreTable() = delete;
	StoreTable(const std::string, const size_t) = delete;
	StoreTable(const StoreTable&) = delete;
	StoreTable& operator=(const StoreTable&) = delete;
	void Initialize(const std::string datadir, const size_t cache_in_mb, uint64_t& last_pkey)=delete;

	/**
	* InsertOne: insert a new record
	*
	* @param record
	*   T* record
	*
	* @param upsert
	*   bool upsert if true
	*
	* @return
	*   bool if inserted
	*/
	bool InsertOne(T* record, bool upsert)
	{
		TransactionT trans;
		bool status = AddRecord(record,&trans,upsert);
		if (status) status = DoTrans(&trans);
		return status;
	}


	/**
	* DeleteOne: delete a record
	*
	* @param record
	*   T* record
	*
	* @return
	*   bool if deleted
	*/
	bool DeleteOne(T* record)
	{
		bool status = GetOne(record,PrimaryKey);
		if (!status || record->notfound()) return false;
		TransactionT trans;
		status = DelRecord(record,&trans);
		if (status) status = DoTrans(&trans);
		return status;
	}

	/**
	* GetOne: get a record by primary or unique key
	*
	* @param record
	*   T* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @return
	*   bool if found
	*/
	bool GetOne(T* record, KeyTypeE keytype)
	{
		bool key_found=false;
		std::string key,value;
		usemydb::Status s ;
		// primary key is direct
		if (keytype==PrimaryKey) {
			key = GetKey(record,PrimaryKey,false);
			key_found=true;
			// check if unique key or index key
		} else if ( unique_keys.find(keytype)!=unique_keys.end() || index_keys.find(keytype)!=index_keys.end()) {
			s = getDB()->Get(usemydb::ReadOptions(), GetKey(record,keytype,false), &value);
			if (s.ok()) {
				NodeT node;
				if ( node.ParseFromString(value) ) {
					record->set_id( node.id() );
					key = GetKey(record,PrimaryKey,false);
					key_found=true;
				}
			}
		} else
			throw identt::BadDataException("Keytype invalid");

		if (!key_found) {
			return false;
		}
		s = getDB()->Get(usemydb::ReadOptions(), key, &value);
		if (!s.ok()) {
			return false;
		}

		if (!record->ParseFromString(value))
			throw identt::BadDataException("Record cannot be parsed");

		return true;
	}


	/**
	* GetMany: get all values by non unique key
	*
	* @param refer
	*   T* reference record for keys
	*
	* @param record map
	*   MapT* records
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param skip
	*   size_t records to skip
	*
	* @param limit
	*   size_t records to fetch
	*
	* @return
	*   bool if found
	*/
	bool GetMany(T* refer, MapT* records, KeyTypeE keytype, size_t skip, size_t limit)
	{
		if (keytype==PrimaryKey) {
			throw identt::BadCodeException("This cannot be used for primary key");
		}
		NodeListT nodes;
		bool key_found=GetKeysFromIndex(keytype,refer, &nodes,skip,limit);
		if (!key_found) return false;
		std::string key,value;
		usemydb::Status s ;
		T record;

		for (size_t i=0; i<nodes.node_size(); ++i) {
			if (nodes.mutable_node(i)->keytype()!=PrimaryKey)
				throw identt::BadDataException("Record is bad ");
			record.set_id( nodes.mutable_node(i)->id() );
			key = GetKey(&record,PrimaryKey,false);
			s = getDB()->Get(usemydb::ReadOptions(), key, &value);
			if (!s.ok())
				throw identt::BadDataException("Record is missing ");
			if (!record.ParseFromString(value))
				throw identt::BadDataException("Record cannot be parsed");
			(*records)[record.id()]=record;
		}

		return true;
	}

	/**
	* AddTrans : add transactions
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @param key
	*   std::string key to populate
	*
	* @param value
	*   std::string& key to populate
	*
	* @param to_del
	*   bool to delete
	*
	*/
	void AddTrans(TransactionT* trans, std::string key, std::string& value, bool to_del)
	{
		TransItemT* item = trans->add_item();
		item->set_key(key);
		if (!to_del) item->set_value(value);
		item->set_to_del(to_del);
	}


	/**
	* DoTrans : do transactions
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @return
	*   bool true if ok
	*/
	bool DoTrans(TransactionT* trans)
	{
		if (trans->item_size()==0) {
			return true;
		}
		usemydb::WriteBatch batch;
		for (auto i=0; i<trans->item_size(); ++i) {
			if (! trans->mutable_item(i)->to_del() ) {
				batch.Put(trans->mutable_item(i)->key(), trans->mutable_item(i)->value() );
			} else {
				batch.Delete(trans->mutable_item(i)->key());
			}
		}
		usemydb::WriteOptions write_options;
		usemydb::Status s = getDB()->Write(write_options, &batch);
		return s.ok();
	}

	/**
	* GetKey: get a  key
	*
	* @param record
	*   T* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param pre
	*   bool pre , whether to get prefix default false
	*
	* @return
	*   std::string key
	*/
	virtual std::string GetKey(T* record, KeyTypeE keytype, bool pre)=0;

	/**
	* GetSecondaryValue: get a secondary key value
	*
	* @param record
	*   T* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param node
	*   NodeT* node to populate
	*
	* @return
	*   bool if exists
	*/
	bool GetSecondaryValue(T* record, KeyTypeE keytype, NodeT* node)
	{
		if ( unique_keys.find(keytype)==unique_keys.end() && index_keys.find(keytype)==index_keys.end() )
			return false;
		std::string key = GetKey(record,keytype,false);
		std::string value;
		usemydb::Status s = getDB()->Get(usemydb::ReadOptions(), key, &value);
		if (!s.ok()) return false;
		return node->ParseFromString(value);
	}

	/**
	* AddRecord : get transaction item
	*
	* @param record
	*   T* reference record for update
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @param upsert
	*   bool upsert if true
	*
	* @return
	*   bool true if added
	*/
	bool AddRecord(T* record, TransactionT* trans, bool upsert)
	{

		if (record->notfound()) return false; // bad data
		if (record->id()==0) return false; // bad data

		T old;
		old.set_id(record->id());
		bool old_record_exists = GetOne(&old,PrimaryKey);
		if (old_record_exists && !upsert) return false; // old data no upsert

		NodeT node;
		bool status;

		// check unique key for clash
		for (auto& keytype : unique_keys) {
			status = GetSecondaryValue(record,keytype,&node);
			if (status && ( record->id() != node.id()) ) return false; // unique key clash
		}

		// node record for dump
		std::string dumpid;
		node.set_id(record->id());
		node.set_keytype(PrimaryKey);
		node.SerializeToString(&dumpid);

		// check unique keys , delete old and update new , keep if same
		for (auto& keytype : unique_keys) {
			std::string u_new = GetKey(record,keytype,false);
			if (old_record_exists) {
				std::string u_old = GetKey(&old,keytype,false);
				if (u_old!=u_new) {
					AddTrans(trans,u_old,dumpid,true); // dumpid ignored
					AddTrans(trans,u_new,dumpid,false);
				}
			} else {
				AddTrans(trans,u_new,dumpid,false);
			}
		}

		// check index keys , delete old and update new , keep if same
		for (auto& keytype : index_keys) {
			std::string u_new = GetKey(record,keytype,false);
			if (old_record_exists) {
				std::string u_old = GetKey(&old,keytype,false);
				if (u_old!=u_new) {
					AddTrans(trans,u_old,dumpid,true); // dumpid ignored
					AddTrans(trans,u_new,dumpid,false);
				}
			} else {
				AddTrans(trans,u_new,dumpid,false);
			}
		}

		// add the record
		record->SerializeToString(&dumpid);
		std::string pkey = GetKey(record,PrimaryKey,false);
		AddTrans(trans,pkey,dumpid,false);

		return true;
	}

	/**
	* DelRecord : get delete transaction item
	*
	* @param record
	*   T* reference record for update
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @return
	*   bool true if deleted
	*/
	bool DelRecord(T* record, TransactionT* trans)
	{
		if (record->notfound()) return false; // bad data

		// insert record for deletion
		std::string value;
		AddTrans(trans,EncodePrimaryKey(PrimaryKey,record->id()),value,true);
		for (auto& keytype : unique_keys) {
			AddTrans(trans,GetKey(record,keytype,false),value,true);
		}
		for (auto& keytype : index_keys) {
			AddTrans(trans,GetKey(record,keytype,false),value,true);
		}
		return true;
	}

	/**
	* GetKeysFromIndex : get primary keys from secondary
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param record
	*   T* reference record for keys
	*
	* @param nodes
	*   NodeListT* node list to fetch
	*
	* @param skip
	*   size_t records to skip
	*
	* @param limit
	*   size_t records to fetch
	*
	* @return
	*   bool true if found
	*/
	bool GetKeysFromIndex(KeyTypeE keytype, T* record, NodeListT* nodes,size_t skip, size_t limit)
	{
		if ( unique_keys.find(keytype)==unique_keys.end() && index_keys.find(keytype)==index_keys.end() )
			return false;
		std::string key = GetKey(record,keytype,true);
		if (key.length() <= IDENTT_KEYID_LEN ) return false;
		std::shared_ptr<usemydb::Iterator> it(getDB()->NewIterator(usemydb::ReadOptions()));
		usemydb::Slice start = usemydb::Slice ( key );
		usemydb::Slice match = usemydb::Slice ( key );
		uint64_t counter=0;
		bool at_least_one=false;
		for (it->Seek(start); it->Valid() && it->key().starts_with(match) ; it->Next()) {
			if (++counter<=skip) continue; // ignore skip
			NodeT* node = nodes->add_node();
			node->ParseFromString(it->value().ToString());
			at_least_one=true;
			if (counter==skip+limit) break; // to handle the id==0 this comes here
		}
		return at_least_one;
	}

	/**
	* ScanTable : scan by primary key
	*
	* @param record map
	*   MapT* records
	*
	* @param startid
	*   size_t first id to fetch
	*
	* @param limit
	*   size_t records to fetch
	*
	* @return
	*   bool if found
	*/
	bool ScanTable(MapT* records, uint64_t startid, size_t limit)
	{
		std::shared_ptr<usemydb::Iterator> it(getDB()->NewIterator(usemydb::ReadOptions()));
		std::string keymatch = EncodeKeyType(PrimaryKey);
		usemydb::Slice match = usemydb::Slice ( keymatch );
		startid = (startid>0) ? startid : 1;
		std::string keystart = EncodePrimaryKey(PrimaryKey,startid);
		usemydb::Slice start = usemydb::Slice ( keystart );

		uint64_t counter=0;
		for (it->Seek(start); it->Valid() && it->key().starts_with(match) ; it->Next()) {
			T record;
			record.ParseFromString(it->value().ToString());
			(*records)[record.id()]=record;
			if (++counter==limit) break;
		}
		return (counter>0);
	}

protected:
	const KeyTypeE PrimaryKey;
	const std::string PrimaryName;
	const KeySetT unique_keys;
	const KeySetT index_keys;


};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_STORE_TABLE_HPP_ */
