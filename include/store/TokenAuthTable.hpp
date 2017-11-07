/**
 * @project identt
 * @file include/store/TokenAuthTable.hpp
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
 *  TokenAuthTable.hpp :   Headers for TokenAuthTable  token auth
 *
 */
#ifndef _IDENTT_STORE_TOKEN_AUTH_TABLE_HPP_
#define _IDENTT_STORE_TOKEN_AUTH_TABLE_HPP_

#include "StoreTable.hpp"

namespace identt {
namespace store {
class TokenAuthTable : public StoreTable<TokenAuthT> {
public:
	using StoreTable<TokenAuthT>::dbpointer;
	using StoreTable<TokenAuthT>::StoreTable;
	using StoreTable<TokenAuthT>::MapT;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	TokenAuthTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~TokenAuthTable() {}

	/**
	* make noncopyable and remove default
	*/

	TokenAuthTable() = delete;
	TokenAuthTable(const std::string, const size_t) = delete;
	TokenAuthTable(const TokenAuthTable&) = delete;
	TokenAuthTable& operator=(const TokenAuthTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   TokenAuthT* record
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
	std::string GetKey(TokenAuthT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_TOKEN_AUTH_TABLE_HPP_ */
