/**
 * @project identt
 * @file src/store/StoreLevel.cc
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
 *  StoreLevel.cc :   store
 *
 */
#define IDENTT_LEVELDB_WRITEBUFFER 32 * IDENTT_MEGABYTE
#define IDENTT_LEVELDB_TBL_BLOCKSIZE 4 * 1024

#define IDENTT_ROCKSDB_WRITEBUFFER 32 * IDENTT_MEGABYTE
#define IDENTT_ROCKSDB_TBL_BLOCKSIZE 4 * 1024 // ssd disk based

#include <store/StoreLevel.hpp>

#ifdef IDENTT_BUILD_WITH_LEVELDB
#include <leveldb/filter_policy.h>
#include <leveldb/table.h>
#include <leveldb/cache.h>
#elif IDENTT_BUILD_WITH_ROCKSDB
#include <rocksdb/filter_policy.h>
#include <rocksdb/table.h>
#include <rocksdb/cache.h>
#endif

/**
* Constructor
*
* @param trydb
*   dbpointer shared pointer to db
*
*/
identt::store::StoreLevel::StoreLevel(dbpointer trydb)
	: db(trydb)
{}

/*
* Initialize : main init
*/
void identt::store::StoreLevel::Initialize(const std::string datadir, const size_t cache_in_mb,uint64_t& last_pkey, uint64_t& last_lkey)
{
	// db options
	usemydb::Options db_options;
	db_options.create_if_missing = true;
#ifdef IDENTT_BUILD_WITH_LEVELDB
	db_options.write_buffer_size = IDENTT_LEVELDB_WRITEBUFFER;
	db_options.block_size = IDENTT_LEVELDB_TBL_BLOCKSIZE;
	db_options.filter_policy = leveldb::NewBloomFilterPolicy(IDENTT_LEN_NODE_KEY);
	db_options.block_cache = leveldb::NewLRUCache(cache_in_mb * IDENTT_MEGABYTE);
#elif IDENTT_BUILD_WITH_ROCKSDB
	// db_options.use_adaptive_mutex = true;
	db_options.write_buffer_size = IDENTT_ROCKSDB_WRITEBUFFER;
	rocksdb::BlockBasedTableOptions tb_options;
	tb_options.block_size = IDENTT_ROCKSDB_TBL_BLOCKSIZE;
	tb_options.filter_policy.reset(rocksdb::NewBloomFilterPolicy(IDENTT_LEN_NODE_KEY));
	tb_options.block_cache = rocksdb::NewLRUCache(cache_in_mb * IDENTT_MEGABYTE);
	// tb_options.cache_index_and_filter_blocks=true;
	db_options.table_factory.reset(rocksdb::NewBlockBasedTableFactory(tb_options));
	// db_options.compression = rocksdb::kSnappyCompression;
#endif

	usemydb::DB* trydb;
	usemydb::Status status = usemydb::DB::Open(db_options, datadir, &trydb);
	if(!status.ok()) {
		throw identt::InitialException("Cannot Open DB: " + status.ToString());
	}
	// add the top fence for data
	for (size_t keytype=K_NONODE; keytype <= K_MAXNODE; ++keytype) {
		std::string value;
		std::string key = EncodePrimaryKey((KeyTypeE)keytype,0);
		status = trydb->Get(usemydb::ReadOptions(), key, &value);
		if (!status.ok()) status = trydb->Put(usemydb::WriteOptions(),key,"");
	}
	if(!status.ok()) {
		throw identt::InitialException("Cannot Write DB: " + status.ToString());
	}
	this->db = dbpointer(trydb);

	std::shared_ptr<usemydb::Iterator> it(getDB()->NewIterator(usemydb::ReadOptions()));
	// get the last used log key
	do {
		uint64_t id;
		std::string value;
		// first key of type
		std::string key = EncodePrimaryKey(K_MAXNODE,0);
		usemydb::Slice start = usemydb::Slice ( key.c_str(), IDENTT_KEYID_LEN );
		std::string prekey = EncodePrimaryKey(K_LOGNODE,0);
		usemydb::Slice match = usemydb::Slice ( prekey.c_str(), IDENTT_KEYID_LEN );
		// check if key exists
		it->Seek(start);
		if (!it->Valid()) throw identt::InitialException("Fixed record K_MAXNODE missing");
		it->Prev();
		if (!it->Valid()) throw identt::InitialException("Zero record before K_MAXNODE missing");
		if (!it->key().starts_with(match)) continue; // has exceeded
		value=it->key().ToString();
		size_t usekey;
		std::tie(usekey,id) = DecodePrimaryKey(value);
		if (id>last_lkey) last_lkey=id;
	} while(false);

	// now get the last used primary key
	for (size_t keytype=K_LOGNODE; keytype > K_NONODE; --keytype) {
		uint64_t id;
		std::string value;
		// first key of type
		std::string key = EncodePrimaryKey((KeyTypeE)keytype,0);
		usemydb::Slice start = usemydb::Slice ( key.c_str(), IDENTT_KEYID_LEN );
		// last key of prev type , -1 is valid as we dont touch K_NONODE
		std::string prekey = EncodePrimaryKey((KeyTypeE)(keytype-1),0);
		usemydb::Slice match = usemydb::Slice ( prekey.c_str(), IDENTT_KEYID_LEN );
		// check if key exists
		it->Seek(start);
		if (!it->Valid()) throw identt::InitialException("Some fixed records missing");
		it->Prev();
		if (!it->Valid()) throw identt::InitialException("Some zero records missing");
		if (!it->key().starts_with(match)) continue; // has exceeded
		value=it->key().ToString();
		size_t usekey;
		std::tie(usekey,id) = DecodePrimaryKey(value);
		if (id>last_pkey) last_pkey=id;
	}
}

/**
* getDB: Get shared pointer to DB
*
* @return
*   dbpointer
*/
identt::store::StoreLevel::dbpointer identt::store::StoreLevel::getDB()
{
	return this->db;
}
