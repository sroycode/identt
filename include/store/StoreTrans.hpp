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

#include <utils/SharedTable.hpp>
#include <store/StoreLevel.hpp>

namespace identt {
namespace store  {
class StoreTrans : public StoreBase {
public:
	using dbpointer = StoreLevel::dbpointer;

	/**
	* Constructor
	*
	*/
	StoreTrans() = default;

	/**
	* make noncopyable and remove default
	*/

	StoreTrans(const StoreTrans&) = delete;
	StoreTrans& operator=(const StoreTrans&) = delete;

	/**
	* Commit : write both transaction and log
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @param is_master
	*   bool is_master
	*
	* @return
	*   none
	*/
	void Commit(::identt::utils::SharedTable::pointer stptr, TransactionT* trans, bool is_master);

	/**
	* CommitLog : write log ensure id is sequentially generated
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @param is_master
	*   bool is_master
	*
	* @return
	*   bool status
	*/
	bool CommitLog(::identt::utils::SharedTable::pointer stptr, TransactionT* trans, bool is_master);

	/**
	* CommitData : write data
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param trans
	*   TransactionT* transaction to handle
	*
	* @return
	*   bool status
	*/
	bool CommitData(::identt::utils::SharedTable::pointer stptr, TransactionT* trans);

	/**
	* ReadLog : read from log in sequence
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param tlist
	*   TransListT* transaction list to handle
	*
	* @return
	*   none
	*/
	void ReadLog(::identt::utils::SharedTable::pointer stptr, TransListT* tlist);

	/**
	* ReadOne : read a single transaction
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param trans
	*   TransactionT* transaction  to handle
	*
	* @return
	*   none
	*/
	void ReadOne(::identt::utils::SharedTable::pointer stptr, TransactionT* trans);

private:

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_STORE_TRANS_HPP_ */
