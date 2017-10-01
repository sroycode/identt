/**
 * @project identt
 * @file include/store/EphemeralPublicKeyTable.hpp
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
 *  EphemeralPublicKeyTable.hpp :   Headers for EphemeralPublicKeyTable  ephemeral public key
 *
 */
#ifndef _IDENTT_STORE_EPHEMERAL_PUBLIC_KEY_TABLE_HPP_
#define _IDENTT_STORE_EPHEMERAL_PUBLIC_KEY_TABLE_HPP_

#include "StoreTable.hpp"

// extern template class identt::store::StoreTable<identt::store::EphemeralPublicKeyT>;

namespace identt {
namespace store {
class EphemeralPublicKeyTable : public StoreTable<EphemeralPublicKeyT> {
public:
	using StoreTable<EphemeralPublicKeyT>::dbpointer;
	using StoreTable<EphemeralPublicKeyT>::StoreTable;
	using StoreTable<EphemeralPublicKeyT>::MapT;

	/**
	* Constructor
	*
	* @param trydb
	*   dbpointer shared pointer to db
	*
	*/
	EphemeralPublicKeyTable(dbpointer trydb);

	/**
	* Destructor
	*
	*/
	virtual ~EphemeralPublicKeyTable() {}

	/**
	* make noncopyable and remove default
	*/

	EphemeralPublicKeyTable() = delete;
	EphemeralPublicKeyTable(const std::string, const size_t) = delete;
	EphemeralPublicKeyTable(const EphemeralPublicKeyTable&) = delete;
	EphemeralPublicKeyTable& operator=(const EphemeralPublicKeyTable&) = delete;


protected:

	/**
	* GetKey: get a key
	*
	* @param record
	*   EphemeralPublicKeyT* record
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
	std::string GetKey(EphemeralPublicKeyT* record, KeyTypeE keytype, bool pre) override;

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_EPHEMERAL_PUBLIC_KEY_TABLE_HPP_ */
