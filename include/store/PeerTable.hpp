/**
 * @project identt
 * @file include/store/PeerTable.hpp
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
 *  PeerTable.hpp :   Headers for PeerTable  peer
 *
 */
#ifndef _IDENTT_STORE_PEER_TABLE_HPP_
#define _IDENTT_STORE_PEER_TABLE_HPP_

#include "StoreTable.hpp"

namespace identt {
namespace store {
class PeerTable : public StoreTable<PeerT> {
public:
	using StoreTable<PeerT>::dbpointer;
	using StoreTable<PeerT>::StoreTable;
	using StoreTable<PeerT>::MapT;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	PeerTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~PeerTable() {}

	/**
	* make noncopyable and remove default
	*/

	PeerTable() = delete;
	PeerTable(const std::string, const size_t) = delete;
	PeerTable(const PeerTable&) = delete;
	PeerTable& operator=(const PeerTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   PeerT* record
	*
	* @param keytype
	*   KeyTypeE key type for index
	*
	* @param pre
	*   bool non unique keys
	*
	* @return
	*   std::string key
	*/
	std::string GetKey(PeerT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_PEER_TABLE_HPP_ */
