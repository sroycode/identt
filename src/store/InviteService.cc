/**
 * @project identt
 * @file src/store/InviteService.cc
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
 *  InviteService.cc : Implementation of Invite Service
 *
 */

#include <store/InviteService.hpp>

#include <query/bin2ascii.h>
#include <utils/sole.hpp>

#include <store/GlobalAssocTable.hpp>
#include <store/EphemeralPublicKeyTable.hpp>
#include <store/InviteTokenTable.hpp>
#include <store/ParAvionTable.hpp>
#include <crypto/CryptoTraits.hpp>

#include <store/StoreTrans.hpp>

/**
* StoreInviteAction : invite query internals
*
*/

void identt::store::InviteService::StoreInviteAction(::identt::utils::SharedTable::pointer stptr,
        identt::query::StoreInviteDataT* inv)
{
	identt::query::InviteQueryT* invqry = inv->mutable_invqry();
	identt::query::InviteResultT* invres = inv->mutable_invres();
	uint64_t currtime = IDENTT_CURRTIME_MS;
	// globalassoc
	::identt::store::GlobalAssocT globalassoc;
	::identt::store::GlobalAssocTable::MapT globalassoc_map;
	::identt::store::GlobalAssocTable globalassoc_table{stptr->maindb.Get()};
	globalassoc.set_medium( invqry->medium() );
	globalassoc.set_address ( invqry->address() );
	bool globalassoc_found = globalassoc_table.GetMany(&globalassoc,&globalassoc_map,
	                         ::identt::store::I_GLOBALASSOC_MEDIUM_ADDRESS,0,1);
	if (globalassoc_found && globalassoc_map.size()>0) {
		throw ::identt::query::SydentException("Binding already known", M_THREEPID_IN_USE);
	}
	// create token
	auto tok = sole::uuid4();
	// create ephemeral keys shared pointer
	auto key = ::identt::crypto::CryptoTraits::Create(THREEPID_DEFAULT_ALGO);
	::identt::store::TransactionT trans;

	// create an entry for token store
	::identt::store::InviteTokenTable token_table{stptr->maindb.Get()};
	::identt::store::InviteTokenT token;
	::identt::store::InviteTokenTable::MapT token_map;
	token.set_token( tok.str() );
	bool tok_found = token_table.GetMany(&token,&token_map,I_INVITETOKEN_TOKEN,0,1);
	if (tok_found && token_map.size()>0)
		throw ::identt::BadDataException("Duplicate token Generated , please retry");
	token.set_id( stptr->maincounter.GetNext() );
	token.set_medium( invqry->medium() );
	token.set_address ( invqry->address() );
	token.set_room_id ( invqry->room_id() );
	token.set_sender ( invqry->sender() );
	token.set_received_ts ( currtime );
	if (!token_table.AddRecord(&token,&trans,false))
		throw ::identt::BadDataException("Cannot Insert token");

	// create an entry for public_key
	::identt::store::EphemeralPublicKeyTable epk_table{stptr->maindb.Get()};
	::identt::store::EphemeralPublicKeyT epk;
	epk.set_public_key( key->GetPublicKey() );
	bool epk_found = epk_table.GetOne(&epk,U_EPHEMERALPUBLICKEY_PUBLICKEY);
	if (epk_found)
		throw ::identt::BadDataException("Duplicate Public Key Generated , please retry");
	epk.set_id( stptr->maincounter.GetNext() );
	epk.set_persistence_ts( currtime );
	epk.set_verify_count( 0 );
	if (!epk_table.AddRecord(&epk,&trans,false))
		throw ::identt::BadDataException("Cannot Insert ephemeral key");

	// create an entry for outgoing
	::identt::store::ParAvionTable paravion_table(stptr->maindb.Get());
	::identt::store::ParAvionT paravion;
	paravion.set_id( stptr->maincounter.GetNext() );
	::identt::mail::MPayloadT* payload = paravion.mutable_payload();
	payload->set_id( paravion.id() );
	payload->set_medium( invqry->medium() );
	payload->set_address( invqry->address() );
	payload->set_actkey ( ::identt::mail::A_ROOMINVITE );
	payload->set_retry( 1 );
	::identt::mail::MPTwoT* mptwo = payload->mutable_mptwo();
	mptwo->set_room_id( invqry->room_id() );
	mptwo->set_sender( invqry->sender() );
	mptwo->set_token( tok.str() );
	mptwo->set_ephemeral_private_key( key->GetSecretKey() );

	if (!paravion_table.AddRecord(&paravion,&trans,false))
		throw ::identt::BadDataException("Cannot Insert paravion");

	// transaction , throws on fail
	::identt::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans,true); // as master

	// return data
	invres->set_token( tok.str() );
	invres->set_public_key( stptr->keyring[ THREEPID_DEFAULT_ALGO_WITH_ID ]->GetPublicKey() );
	// add system public key
	auto nkey = invres->add_public_keys();
	nkey->set_key_validity_url( stptr->baseurl.Get() + "/_matrix/identity/api/v1/pubkey/isvalid" );
	nkey->set_public_key( invres->public_key() );
	// add ephemeral key
	nkey = invres->add_public_keys();
	nkey->set_key_validity_url( stptr->baseurl.Get() + "_matrix/identity/api/v1/pubkey/ephemeral/isvalid" );
	nkey->set_public_key( key->GetPublicKey() );
}
