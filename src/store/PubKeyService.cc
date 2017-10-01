/**
 * @project identt
 * @file src/store/PubKeyService.cc
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
 *  PubKeyService.cc : Implementation of PubKey Service
 *
 */

#include <store/PubKeyService.hpp>

#include <query/bin2ascii.h>
#include <store/EphemeralPublicKeyTable.hpp>

/**
* GetPubKeyAction : Service Endpoint GetPubKey
*
*/
void identt::store::PubKeyService::GetPubKeyAction(::identt::utils::SharedTable::pointer stptr, ::identt::query::PubKeyT* pubkey)
{

	// action
	std::string algo = pubkey->algo() + std::string(":") + pubkey->identifier() ;
	if (stptr->keyring.find(algo) != stptr->keyring.end()) {
		pubkey->Clear();
		pubkey->set_public_key( stptr->keyring[algo]->GetPublicKey() );
	}
}

/**
* GetPubKeyValidAction : Service Endpoint GetPubKeyValid
*
*/
void identt::store::PubKeyService::GetPubKeyValidAction(::identt::utils::SharedTable::pointer stptr, ::identt::query::PubKeyT* pubkey)
{

	// action
	std::string tocom = pubkey->public_key();
	// pubkey->Clear();
	pubkey->set_valid(false);
	for (::identt::utils::SharedTable::KeyRingT::const_iterator it = stptr->keyring.begin(); it!= stptr->keyring.end(); ++it) {
		if ( it->second->GetPublicKey() == tocom ) {
			pubkey->set_valid(true);
			return;
		}
	}
}

/**
* GetEphemeralValidAction : Service Endpoint GetEphemeralValid
*
*/
void identt::store::PubKeyService::GetEphemeralValidAction(::identt::utils::SharedTable::pointer stptr, ::identt::query::PubKeyT* pubkey)
{

	// action
	::identt::store::EphemeralPublicKeyTable epk_table{stptr->getDB()};
	::identt::store::EphemeralPublicKeyT epk;
	epk.set_public_key( pubkey->public_key() );
	bool epk_found = epk_table.GetOne(&epk,U_EPHEMERALPUBLICKEY_PUBLICKEY);
	if (epk_found)
		pubkey->set_valid(true);
}
