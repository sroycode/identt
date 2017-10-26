/**
 * @project identt
 * @file src/store/ThreePidService.cc
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
 *  ThreePidService.cc : Implementation of ThreePid Service
 *
 */

#include <store/ThreePidService.hpp>

#include <query/bin2ascii.h>
#include <query/SignedJson.hpp>

#include <store/ValidationSessionTable.hpp>
#include <store/GlobalAssocTable.hpp>
#include <store/LocalAssocTable.hpp>
#include <store/InviteTokenTable.hpp>
#include <store/ParAvionTable.hpp>

#include <store/StoreTrans.hpp>

#include <set>
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"


/**
* GetValidated3pidAction: Validate a 3pid Request
*
*/
void identt::store::ThreePidService::GetValidated3pidAction(
    ::identt::utils::SharedTable::pointer stptr,::identt::query::GetValidated3pidDataT* gva)
{

	::identt::query::SubmitTokenT* subtok = gva->mutable_subtok();
	::identt::query::ValidatedAtT* valresult = gva->mutable_valresult();
	// check
	uint64_t sid = subtok->sid();
	uint64_t currtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if (sid==0)
		throw ::identt::query::SydentException("sid value required", M_MISSING_PARAMS);
	if (subtok->client_secret()=="")
		throw ::identt::query::SydentException("client_secret value required", M_MISSING_PARAMS);
	// start
	::identt::store::ValidationSessionT valsession;
	valsession.set_id ( sid );
	::identt::store::ValidationSessionTable valsession_table{stptr->maindb.Get()};
	bool validation_found = valsession_table.GetOne(&valsession,::identt::store::K_VALIDATIONSESSION);
	if (!validation_found || valsession.client_secret() != subtok->client_secret())
		throw ::identt::query::SydentException("No valid session was found matching that sid and client secret",M_NO_VALID_SESSION);
	if (valsession.validated() <=0) {
		// this will throw anyway
		if (THREEPID_SESSION_VALID_LIFETIME_MS + valsession.mtime() > currtime )
			throw ::identt::query::SydentException("This validation session has expired: call requestToken again",M_SESSION_EXPIRED);
		throw ::identt::query::SydentException("This validation session has not yet been completed",M_SESSION_NOT_VALIDATED);
	}
	valresult->set_medium( valsession.medium() );
	valresult->set_address ( valsession.address() );
	valresult->set_validated_at ( valsession.mtime() );
}

/**
* Bind3pidAction: Bind a 3pid Request
*
*/
void identt::store::ThreePidService::Bind3pidAction(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::Bind3pidDataT* bpa)
{

	// check
	::identt::query::SubmitTokenT* subtok = bpa->mutable_subtok();
	uint64_t sid = subtok->sid();
	uint64_t currtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if (sid==0)
		throw ::identt::query::SydentException("sid value required", M_MISSING_PARAMS);
	if (subtok->client_secret()=="")
		throw ::identt::query::SydentException("client_secret value required", M_MISSING_PARAMS);
	if (subtok->mxid()=="")
		throw ::identt::query::SydentException("mxid value required", M_MISSING_PARAMS);
	// start
	::identt::store::ValidationSessionT valsession;
	valsession.set_id ( sid );
	::identt::store::ValidationSessionTable valsession_table{stptr->maindb.Get()};
	bool validation_found = valsession_table.GetOne(&valsession,::identt::store::K_VALIDATIONSESSION);
	if (!validation_found || valsession.client_secret() != subtok->client_secret())
		throw ::identt::query::SydentException("No valid session was found matching that sid and client secret",M_NO_VALID_SESSION);
	if (valsession.validated() <=0) {
		// this will throw anyway
		if (THREEPID_SESSION_VALID_LIFETIME_MS + valsession.mtime() > currtime )
			throw ::identt::query::SydentException("This validation session has expired: call requestToken again",M_SESSION_EXPIRED);
		throw ::identt::query::SydentException("This validation session has not yet been completed",M_SESSION_NOT_VALIDATED);
	}
	// work starts here
	std::string origin_server = stptr->hostname.Get();
	::identt::store::TransactionT trans;
	uint64_t expires = THREEPID_SESSION_VALID_LIFETIME_MS + currtime;
	// localassoc
	::identt::store::LocalAssocT localassoc;
	::identt::store::LocalAssocTable localassoc_table{stptr->maindb.Get()};
	localassoc.set_medium( valsession.medium() );
	localassoc.set_address ( valsession.address() );
	bool localassoc_found = localassoc_table.GetOne(&localassoc,::identt::store::U_LOCALASSOC_MEDIUM_ADDRESS);
	// uncomment next line to disallow transfers
	// if (localassoc_found) throw ::identt::query::BadQueryException("This threepid is already associated");

	// blindly update local
	if (!localassoc_found) localassoc.set_id( stptr->maincounter.GetNext() );
	localassoc.set_mxid( subtok->mxid() );
	localassoc.set_ts( currtime );
	localassoc.set_not_before( currtime );
	localassoc.set_not_after ( expires );
	bool status = localassoc_table.AddRecord(&localassoc, &trans,localassoc_found);
	if (!status) throw ::identt::BadDataException("Cannot Insert local 3pid assoc");
	// globalassoc
	::identt::store::GlobalAssocT globalassoc;
	::identt::store::GlobalAssocTable globalassoc_table{stptr->maindb.Get()};
	globalassoc.set_origin_id( localassoc.id() );
	globalassoc.set_origin_server( origin_server );
	bool globalassoc_found = globalassoc_table.GetOne(&globalassoc,::identt::store::U_GLOBALASSOC_ORIGINSERVER_ORIGINID);
	if (!globalassoc_found) globalassoc.set_id( stptr->maincounter.GetNext() );
	globalassoc.set_medium( valsession.medium() );
	globalassoc.set_address ( valsession.address() );
	globalassoc.set_mxid( subtok->mxid() );
	globalassoc.set_ts( currtime );
	globalassoc.set_not_before( currtime );
	globalassoc.set_not_after ( expires );
	globalassoc.clear_sg_assoc(); // remove the signed part

	// signed json
	std::string tosign;
	// set owners
	::identt::query::PubKeyT pubkey;
	pubkey.set_owner(origin_server);
	pubkey.set_algo(THREEPID_DEFAULT_ALGO);
	pubkey.set_identifier(THREEPID_DEFAULT_ALGO_ID);

	// add signature
	::identt::query::SignedJson::SignatureListT signatures;
	this->AddSign(stptr, &globalassoc , &pubkey, tosign,signatures);
	globalassoc.set_sg_assoc ( tosign );

	status = globalassoc_table.AddRecord(&globalassoc, &trans,globalassoc_found);
	if (!status) throw ::identt::BadDataException("Cannot Insert global 3pid assoc");

	// set result
	::identt::query::ValidatedInviteT result;
	result.set_medium( valsession.medium() );
	result.set_address ( valsession.address() );
	result.set_validated_at( currtime );

	// invite-tokens start
	::identt::store::InviteTokenTable token_table{stptr->maindb.Get()};
	::identt::store::InviteTokenT token;
	::identt::store::InviteTokenTable::MapT token_map;
	std::set<std::pair<std::string,std::string> > dup_set;
	token.set_medium( valsession.medium() );
	token.set_address ( valsession.address() );
	bool tok_found = token_table.GetMany(&token,&token_map,I_INVITETOKEN_MEDIUM_ADDRESS,0,99999);
	for (auto it = token_map.begin(); it!=token_map.end(); ++it) {
		auto& mytok = it->second;
		// if (mytok.sent_ts()>0) continue; // shreos
		// set time
		mytok.set_sent_ts( currtime);
		// add for processing before duplicate check
		if (!token_table.AddRecord(&mytok,&trans,true))
			throw ::identt::BadDataException("Cannot update token");
		// try to insert if fails duplicate
		auto insert_ok = dup_set.insert({valsession.medium(),valsession.address()});
		if (!insert_ok.second) continue; // value already there
		// add to invite
		::identt::query::InviteItemT* invite = result.add_invites();
		// import from invite
		invite->set_mxid ( subtok->mxid() );
		invite->set_token ( mytok.token() );
		invite->set_id ( mytok.id() );
		invite->set_medium ( mytok.medium() );
		invite->set_address ( mytok.address() );
		invite->set_room_id ( mytok.room_id() );
		invite->set_sender ( mytok.sender() );
		invite->set_received_ts ( mytok.received_ts() );
		invite->set_sent_ts ( mytok.sent_ts() );
	}
	tosign.clear();
	::identt::query::pb2json(&result , tosign);

	// convert to json and sign pieces
	rapidjson::Document d;
	d.Parse(tosign.c_str());
	if (d.HasParseError())
		throw ::identt::BadDataException("Json failed");
	if ( d["invites"].IsArray() ) {
		::identt::query::MxidSignedT sg;
		for (auto& v : d["invites"].GetArray()) {
			sg.Clear();
			if (v.HasMember("mxid"))
				sg.set_mxid ( v["mxid"].GetString() );
			if (v.HasMember("token"))
				sg.set_token ( v["token"].GetString() );
			std::string tmpstr;
			this->AddSign(stptr, &sg , &pubkey, tmpstr,signatures);
			rapidjson::Document e;
			e.Parse(tmpstr.c_str());
			if (!e.IsObject()) continue;
			v.AddMember("signed", rapidjson::Value(rapidjson::kObjectType), d.GetAllocator() );
			for (auto itr = e.MemberBegin(); itr != e.MemberEnd(); ++itr) {
				v["signed"].AddMember(itr->name, itr->value, d.GetAllocator() );
			}
		}
	}

	// json to string
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string tmpstr;
	tmpstr.append(buffer.GetString(), buffer.GetSize());
	std::string output = bpa->output();
	this->AddSign(stptr, tmpstr , &pubkey, output,signatures);
	bpa->set_output( output );

	// transaction , throws on fail
	::identt::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans);

}


