/**
 * @project identt
 * @file include/store/InviteTokenTable.hpp
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
 *  InviteTokenTable.hpp :   Headers for InviteTokenTable  invite token
 *
 */
#ifndef _IDENTT_STORE_INVITE_TOKEN_TABLE_HPP_
#define _IDENTT_STORE_INVITE_TOKEN_TABLE_HPP_

#include "StoreTable.hpp"

namespace identt {
namespace store {
class InviteTokenTable : public StoreTable<InviteTokenT> {
public:
	using StoreTable<InviteTokenT>::dbpointer;
	using StoreTable<InviteTokenT>::StoreTable;
	using StoreTable<InviteTokenT>::MapT;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	InviteTokenTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~InviteTokenTable() {}

	/**
	* make noncopyable and remove default
	*/

	InviteTokenTable() = delete;
	InviteTokenTable(const std::string, const size_t) = delete;
	InviteTokenTable(const InviteTokenTable&) = delete;
	InviteTokenTable& operator=(const InviteTokenTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   InviteTokenT* record
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
	std::string GetKey(InviteTokenT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_INVITE_TOKEN_TABLE_HPP_ */
