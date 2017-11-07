/**
 * @project identt
 * @file src/store/StoreTrans.cc
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
 *  StoreTrans.cc :   Implementation for transaction store
 *
 */

#include <store/StoreTrans.hpp>
#include <hrpc/HrpcClient.hpp>

/**
* Commit : write both transaction and log
*
*/
void ::identt::store::StoreTrans::StoreTrans::Commit(::identt::utils::SharedTable::pointer stptr, TransactionT* trans, bool is_master)
{
	if (trans->item_size()==0) return;
	if (!CommitLog(stptr,trans,is_master))
		throw ::identt::BadDataException("Insert failed for log");
	if (!CommitData(stptr,trans))
		throw ::identt::BadDataException("Insert failed for data");
	// replicate this data on lastslave
	auto lastslave = stptr->lastslave.Get();
	if (is_master && stptr->is_master.Get() && (lastslave.length()>0)) {
		identt::hrpc::HrpcClient hclient;
		bool status = hclient.SendToRemote(stptr,lastslave,::identt::hrpc::R_BUFFTRANS,trans,true);
		if (!status) stptr->lastslave.Set("");
	}
}

/**
* CommitLog : write log ensure id is sequentially generated
*
*/
bool ::identt::store::StoreTrans::StoreTrans::CommitLog(::identt::utils::SharedTable::pointer stptr, TransactionT* trans, bool is_master)
{
	if (trans->item_size()==0) return true;
	dbpointer logdb = stptr->logdb.Get();
	// the logcounter is incremented just before writing
	trans->set_id( stptr->logcounter.GetNext() );
	if (is_master) trans->set_ts( IDENTT_CURRTIME_MS );
	std::string value;
	trans->SerializeToString(&value);
	usemydb::Status s = logdb->Put(usemydb::WriteOptions(),EncodePrimaryKey(K_LOGNODE,trans->id()),value);
	return s.ok();
}

/**
* CommitData : write data
*
*/
bool ::identt::store::StoreTrans::StoreTrans::CommitData(::identt::utils::SharedTable::pointer stptr, TransactionT* trans)
{
	if (trans->item_size()==0) return true;
	usemydb::WriteBatch batch;
	for (auto i=0; i<trans->item_size(); ++i) {
		if (! trans->mutable_item(i)->to_del() ) {
			batch.Put(trans->mutable_item(i)->key(), trans->mutable_item(i)->value() );
		} else {
			batch.Delete(trans->mutable_item(i)->key());
		}
	}
	dbpointer maindb = stptr->maindb.Get();
	usemydb::Status s = maindb->Write(usemydb::WriteOptions(), &batch);
	return s.ok();
}

/**
* ReadLog : read from log in sequence
*
*/
void ::identt::store::StoreTrans::StoreTrans::ReadLog(::identt::utils::SharedTable::pointer stptr, TransListT* tlist)
{
	auto logc = stptr->logcounter.Get();
	if (tlist->lastid()>logc)
		throw ::identt::BadDataException("Remote Log Counter ahead");

	// if no change return
	if (tlist->lastid()==logc) {
		tlist->set_ts( IDENTT_CURRTIME_MS );
		tlist->set_currid( logc );
		tlist->set_limit( 0 );
		return;
	}

	dbpointer logdb = stptr->logdb.Get();
	std::shared_ptr<usemydb::Iterator> it(logdb->NewIterator(usemydb::ReadOptions()));
	// start from lastid and skip first, if zero will hit and skip fence
	std::string keystart = EncodePrimaryKey(K_LOGNODE,tlist->lastid());
	usemydb::Slice start = usemydb::Slice ( keystart.c_str(), IDENTT_LEN_NODE_KEY);
	usemydb::Slice match = usemydb::Slice ( keystart.c_str(), IDENTT_KEYID_LEN ); // only this keytype

	uint64_t counter=0;

	for (it->Seek(start); it->Valid() && it->key().starts_with(match) ; it->Next()) {
		TransactionT record;
		if (!record.ParseFromString(it->value().ToString())) continue;
		DLOG(INFO) << " ### " << tlist->lastid() << " -- " << record.id();
		if (record.id() == tlist->lastid()) continue; // skips the first
		if ((tlist->lastid()+1)!=record.id() ) break; // out of sequence
		tlist->set_lastid( record.id() );
		record.Swap(tlist->add_trans());
		if (++counter > tlist->limit() ) break; // break on exceed
	}
	tlist->set_currid( stptr->logcounter.Get() );
	tlist->set_ts( IDENTT_CURRTIME_MS );
	if (counter) tlist->set_limit( counter-1 );
}

/**
* ReadOne : read a single transaction
*
*/
void identt::store::StoreTrans::ReadOne(::identt::utils::SharedTable::pointer stptr, TransactionT* trans)
{
	std::string value;
	dbpointer logdb = stptr->logdb.Get();
	auto s = logdb->Get(usemydb::ReadOptions(), EncodePrimaryKey(K_LOGNODE,trans->id()), &value);
	if (!s.ok()) {
		trans->set_notfound( true );
		return;
	}
	if (! trans->ParseFromString(value) ) trans->set_notfound( true );
}

