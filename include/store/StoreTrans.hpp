/**
 * @project identt
 * @file include/store/StoreTrans.hpp
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
 *  StoreTrans.hpp :   Headers for transaction store
 *
 */
#ifndef _IDENTT_STORE_STORE_TRANS_HPP_
#define _IDENTT_STORE_STORE_TRANS_HPP_

#include <store/StoreLevel.hpp>

namespace identt {
namespace store  {
class StoreTrans : public StoreBase {
public:
	using dbpointer = StoreLevel::dbpointer;

	/**
	* Constructor
	*
	* @param maindb
	*   dbpointer shared pointer to log db
	*
	* @param logdb
	*   dbpointer shared pointer to log db
	*
	*/
	StoreTrans(dbpointer maindb_, dbpointer logdb_) :
		maindb(maindb_),
		logdb(logdb_) {}

	/**
	* make noncopyable and remove default
	*/

	StoreTrans() = delete;
	StoreTrans(const StoreTrans&) = delete;
	StoreTrans& operator=(const StoreTrans&) = delete;

	/**
	* Commit : write both transaction and log
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @return
	*   none
	*/
	void Commit(TransactionT* trans)
	{
		if (!CommitLog(trans))
				throw ::identt::BadDataException("Insert failed for log");
		if (!CommitData(trans))
				throw ::identt::BadDataException("Insert failed for data");
	}

	/**
	* CommitLog : write log
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @return
	*   bool status
	*/
	bool CommitLog(TransactionT* trans)
	{
		if (trans->item_size()==0) return true;
		std::string value;
		trans->SerializeToString(&value);
		usemydb::Status s = logdb->Put(usemydb::WriteOptions(),EncodePrimaryKey(K_LOGNODE,trans->id()),value);
		return s.ok();
	}

	/**
	* CommitData : write data
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @return
	*   bool status
	*/
	bool CommitData(TransactionT* trans)
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
		usemydb::Status s = maindb->Write(usemydb::WriteOptions(), &batch);
		return s.ok();
	}

private:
	dbpointer maindb;
	dbpointer logdb;

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_STORE_TRANS_HPP_ */
