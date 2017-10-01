/**
 * @project identt
 * @file include/store/PubKeyService.hpp
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
 *  PubKeyService.hpp : Header for pubkey service service
 *
 */
#ifndef _IDENTT_STORE_PUBKEY_SERVICE_HPP_
#define _IDENTT_STORE_PUBKEY_SERVICE_HPP_

#include <query/SignedJson.hpp> // includes QueryBase
#include <store/StoreBase.hpp>

namespace identt {
namespace store {

class PubKeyService : public StoreBase, public ::identt::query::SignedJson {
public:

	/**
	* GetPubKeyAction : Service Endpoint GetPubKey
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param pubkey
	*   ::identt::query::PubKeyT* pubkey
	*
	* @return
	*   none
	*/
	void GetPubKeyAction(::identt::utils::SharedTable::pointer stptr, ::identt::query::PubKeyT* pubkey);

	/**
	* GetPubKeyValidAction : Service Endpoint GetPubKeyValid
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param pubkey
	*   ::identt::query::PubKeyT* pubkey
	*
	* @return
	*   none
	*/
	void GetPubKeyValidAction(::identt::utils::SharedTable::pointer stptr, ::identt::query::PubKeyT* pubkey);

	/**
	* GetEphemeralValidAction : Service Endpoint GetEphemeralValid
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param pubkey
	*   ::identt::query::PubKeyT* pubkey
	*
	* @return
	*   none
	*/
	void GetEphemeralValidAction(::identt::utils::SharedTable::pointer stptr, ::identt::query::PubKeyT* pubkey);

};
} // namespace store
} // namespace identt

#endif // _IDENTT_STORE_PUBKEY_SERVICE_HPP_
