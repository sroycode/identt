/**
 * @project identt
 * @file src/store/ValidateService.cc
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
 *  ValidateService.cc : Implementation of Validate Service
 *
 */

#include <store/ValidateService.hpp>

#include <query/bin2ascii.h>
#include <store/ValidationSessionTable.hpp>
#include <store/TokenAuthTable.hpp>
#include <store/ParAvionTable.hpp>

#include <store/StoreTrans.hpp>

/**
* RequestTokenAction : Service Endpoint RequestToken
*
*/
void identt::store::ValidateService::RequestTokenAction(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::RequestTokenDataT* rtoka)
{

	// action
	uint64_t currtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	::identt::query::RequestTokenT* reqtok = rtoka->mutable_reqtok();
	::identt::query::SuccessSidT* ssid = rtoka->mutable_ssid();
	std::string medium = rtoka->medium();

	::identt::store::TransactionT trans;
	::identt::store::ValidationSessionT valsession;
	::identt::store::ValidationSessionTable valsession_table{stptr->maindb.Get()};

	std::string address;
	PopulateAddress(reqtok,medium,address);

	valsession.set_medium( medium );
	valsession.set_address( address );
	valsession.set_client_secret( reqtok->client_secret() );

	// fetch existing else insert new

	bool validation_found = valsession_table.GetOne(&valsession,::identt::store::U_VALIDATIONSESSION_MEDIUM_ADDRESS_CLIENTSECRET);
	if (validation_found) {
		// nothing
	} else {
		valsession.set_id( stptr->maincounter.GetNext() );
		valsession.set_mtime( currtime );
		if (!valsession_table.AddRecord(&valsession,&trans,false))
			throw ::identt::BadDataException("Cannot Insert vsession");
	}

	// get token auth if exists
	::identt::store::TokenAuthTable tokenauth_table(stptr->maindb.Get());
	::identt::store::TokenAuthT tokenauth;
	tokenauth.set_validation_session( valsession.id() );
	bool tokenauth_found = tokenauth_table.GetOne(&tokenauth, ::identt::store::U_TOKENAUTH_VALIDATIONSESSION);
	// generate a new token if send_attempt_number is higher 
	bool generate_token = (!tokenauth_found);
	if (tokenauth_found) {
		if (( reqtok->send_attempt() - tokenauth.send_attempt_number() ) >1)
			throw ::identt::BadDataException("Token out of Sync");
		if (( tokenauth.send_attempt_number() - reqtok->send_attempt()) ==1)
			generate_token=true;
	}
	if (generate_token) {
		tokenauth.set_send_attempt_number( reqtok->send_attempt() );
		// set id for new record
		if (!tokenauth_found)
			tokenauth.set_id( stptr->maincounter.GetNext() );
		uint64_t tok = currtime % 999999;
		tokenauth.set_token( std::to_string(tok) );
		// tokenauth.set_send_attempt_number( -1 );
		if (!tokenauth_table.AddRecord(&tokenauth,&trans,false))
			throw ::identt::BadDataException("Cannot Insert tokenauth");

		// create an entry for outgoing
		::identt::store::ParAvionTable paravion_table(stptr->maindb.Get());
		::identt::store::ParAvionT paravion;
		paravion.set_id( stptr->maincounter.GetNext() );
		::identt::mail::MPayloadT* payload = paravion.mutable_payload();
		payload->set_id( paravion.id() );
		payload->set_medium( medium );
		payload->set_address( address );
		payload->set_actkey ( ::identt::mail::A_AUTHTOKEN );
		payload->set_retry( reqtok->send_attempt() );
		::identt::mail::MPOneT* mpone = payload->mutable_mpone();
		mpone->set_sid( valsession.id() );
		mpone->set_country( reqtok->country() );

		if (!paravion_table.AddRecord(&paravion,&trans,false))
			throw ::identt::BadDataException("Cannot Insert paravion");

		// final step , transaction throws on fail
		::identt::store::StoreTrans storetrans;
		storetrans.Commit(stptr,&trans);
	}

	LOG(INFO) << "token=\"" <<  tokenauth.token() << "\" sid=" <<  valsession.id();
	ssid->set_success( true);
	ssid->set_sid( valsession.id() );
	if (medium=="msisdn") {
		ssid->set_msisdn( address );
		ssid->set_intl_fmt( "+" + address );
	}
}

/**
* SubmitTokenAction : Service Endpoint SubmitToken
*
*/
void identt::store::ValidateService::SubmitTokenAction(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::SubmitTokenDataT* stoka)
{
	// action

	uint64_t currtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	::identt::query::SubmitTokenT* subtok = stoka->mutable_subtok();
	::identt::query::ErrorT* result = stoka->mutable_result();
	std::string medium = stoka->medium();

	::identt::store::TokenAuthT tokenauth;
	tokenauth.set_validation_session( subtok->sid() );
	::identt::store::TokenAuthTable tokenauth_table(stptr->maindb.Get());
	bool found = tokenauth_table.GetOne(&tokenauth, ::identt::store::U_TOKENAUTH_VALIDATIONSESSION);
	if (!found)
		throw ::identt::query::SydentException("Sid not found", M_NO_VALID_SESSION);
	if (tokenauth.token() != subtok->token())
		throw ::identt::query::SydentException("Token does not match ", M_NO_VALID_SESSION);
	::identt::store::ValidationSessionT valsession;
	::identt::store::ValidationSessionTable valsession_table{stptr->maindb.Get()};

	valsession.set_id( tokenauth.validation_session() );
	found = valsession_table.GetOne(&valsession,::identt::store::K_VALIDATIONSESSION);
	if (!found)
		throw ::identt::query::SydentException("Session not found", M_NO_VALID_SESSION);

	if (valsession.client_secret() != subtok->client_secret())
		throw ::identt::query::SydentException("Client secret does not match the one given when requesting the token",
		                                       M_INCORRECT_CLIENT_SECRET);

	if (currtime - valsession.mtime() > THREEPID_SESSION_VALID_LIFETIME_MS)
		throw ::identt::query::SydentException("This validation session has expired", M_SESSION_EXPIRED);

	if (valsession.validated()>0) {
		result->set_success(true);
		result->set_error("This session is already validated");
		return;
	}

	::identt::store::TransactionT trans;
	valsession.set_validated( 1 );
	valsession.set_mtime( currtime );
	if (!valsession_table.AddRecord(&valsession,&trans,true))
		throw ::identt::BadDataException("Cannot Insert vsession");
	// transaction , throws on fail
	::identt::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans);
	// set
	result->set_success(true);
	result->set_error("This session is now validated");
}

/**
* PopulateAddress : Validate RequestToken medium and populate address
*
*/
void identt::store::ValidateService::PopulateAddress( ::identt::query::RequestTokenT* reqtok, const std::string& medium, std::string& address)
{
	if (medium=="email") {
		address=reqtok->email();
		std::transform(address.begin(), address.end(), address.begin(), ::tolower);
		if (address.length()<5)
			throw ::identt::query::SydentException("Invalid Email", M_INVALID_EMAIL);
	} else {
		std::string country = address=reqtok->country();
		std::string phone_number = address=reqtok->phone_number();
		if (phone_number.length()<5)
			throw ::identt::query::SydentException("Invalid Phone No", M_INVALID_PHONE_NUMBER);
		if (country.length()==0) {
			if (phone_number.substr(0,1)=="+") phone_number = phone_number.substr(1);
			else if (phone_number.substr(0,2)=="00") phone_number = phone_number.substr(2);
			else throw ::identt::query::SydentException("Invalid Phone No without country", M_INVALID_PHONE_NUMBER);
			auto it = ::identt::query::ValidIsdCodes.find(phone_number.substr(0,2));
			if (it==::identt::query::ValidIsdCodes.end())
				throw ::identt::query::SydentException("This country code is not supported", M_INVALID_PHONE_NUMBER);
			address=phone_number;
			return; // stripped the no
		} else if (country.length()==2) {
			auto it = ::identt::query::ValidIsdCodes.find(country);
			if (it==::identt::query::ValidIsdCodes.end())
				throw ::identt::query::SydentException("This country code is not supported", M_INVALID_PHONE_NUMBER);
			// check if isd code is there in phone no and matches country
			if (phone_number.substr(0,1)=="+") phone_number = phone_number.substr(1);
			else if (phone_number.substr(0,2)=="00") phone_number = phone_number.substr(2);
			else {
				address=it->second + phone_number;
				return; // phone no has no country code so we added it
			}
			// the else and else if need validation ?
			if (phone_number.substr(0,2)!=it->second)
				throw ::identt::query::SydentException("Number has different country code", M_INVALID_PHONE_NUMBER);
			address=phone_number;
			return; // phone no country code retained
		} else {
			throw ::identt::query::SydentException("Only 2 letter country code supported", M_INVALID_PHONE_NUMBER);
		}
	}
}
