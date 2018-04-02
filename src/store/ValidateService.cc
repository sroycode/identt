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
	uint64_t currtime = IDENTT_CURRTIME_MS;
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

	bool validation_expired=false;
	bool validation_found = valsession_table.GetOne(&valsession,::identt::store::U_VALIDATIONSESSION_MEDIUM_ADDRESS_CLIENTSECRET);
	if (validation_found) {
		// expired session rekindle
		if (currtime - valsession.mtime() > THREEPID_SESSION_VALID_LIFETIME_MS) {
			validation_expired=true;
			valsession.set_mtime( currtime );
			if (!valsession_table.AddRecord(&valsession,&trans,true))
				throw ::identt::BadDataException("Cannot Insert vsession");
		}
	}
	else {
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
	// generate token if no tokenauth , validation expired or send_attempt higher
	bool generate_token = ( (!tokenauth_found) || validation_expired || (reqtok->send_attempt() > tokenauth.send_attempt_number()) );

	if (generate_token) {
		tokenauth.set_send_attempt_number( reqtok->send_attempt() );
		// set id for new record
		if (!tokenauth_found)
			tokenauth.set_id( stptr->maincounter.GetNext() );

		std::string tok_str = tokenauth.token() ;

		// if new or validation_expired make new token
		if ( (!tokenauth_found) || validation_expired ) {
			std::stringstream ss;
			ss << std::setfill ('1') << std::setw (6) << (currtime % 999999);
			tok_str = ss.str();
		}
		tokenauth.set_token( tok_str );

		// tokenauth.set_send_attempt_number( -1 );
		if (!tokenauth_table.AddRecord(&tokenauth,&trans,tokenauth_found)) // tokenauth_found is false if new
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
		mpone->set_sid( std::to_string(valsession.id()) );
		mpone->set_country( reqtok->country() );
		mpone->set_token( tok_str );

		if (!paravion_table.AddRecord(&paravion,&trans,false))
			throw ::identt::BadDataException("Cannot Insert paravion");

		// final step , transaction throws on fail
		::identt::store::StoreTrans storetrans;
		storetrans.Commit(stptr,&trans,true); // as master
	}

	// pass back token
	rtoka->set_token( tokenauth.token() );

	ssid->set_success( true);
	ssid->set_sid( std::to_string(valsession.id()) );
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

	uint64_t currtime = IDENTT_CURRTIME_MS;

	::identt::query::SubmitTokenT* subtok = stoka->mutable_subtok();
	::identt::query::ErrorT* result = stoka->mutable_result();
	std::string medium = stoka->medium();

	::identt::store::TokenAuthT tokenauth;
	tokenauth.set_validation_session( std::stoul( subtok->sid() ) );
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
	storetrans.Commit(stptr,&trans,true); // as master
	// set
	result->set_success(true);
	result->set_error("This session is now validated");
}

/**
* PopulateAddress : Validate RequestToken medium and populate address
*
*/
void identt::store::ValidateService::PopulateAddress(::identt::query::RequestTokenT* reqtok,
        const std::string& medium, std::string& address)
{
	if (medium=="email") {
		address=reqtok->email();
		std::transform(address.begin(), address.end(), address.begin(), ::tolower);
		if (address.length()<5)
			throw ::identt::query::SydentException("Invalid Email", M_INVALID_EMAIL);
		return;
	}
	else if (medium=="msisdn") {
		std::string country = reqtok->country();
		std::string phone_number = address=reqtok->phone_number();
		if (phone_number.length()<5)
			throw ::identt::query::SydentException("Invalid Phone No", M_INVALID_PHONE_NUMBER);
		/**
				// if no country code try detecting it from phone no
				if (country.length()==0) {
					// normalize for isd containing nos
					if (phone_number.substr(0,1)=="+") phone_number = phone_number.substr(1);
					else if (phone_number.substr(0,2)=="00") phone_number = phone_number.substr(2);
					else throw ::identt::query::SydentException("cannot detect country code", M_INVALID_PHONE_NUMBER);

					// check if in our list
					// dont know if this is a good idea, usa canada share id codes
					for (const auto& p : identt::query::ValidIsdCodes ) {
						if (phone_number.substr(0,p.second.length())==p.second) {
							reqtok->set_country(p.first);
							address=phone_number;
							return;
						}
					}
					throw ::identt::query::SydentException("cannot detect country code", M_INVALID_PHONE_NUMBER);
				}
		*/
		if (country.length()!=2)
			throw ::identt::query::SydentException("Only 2 letter country code supported", M_INVALID_PHONE_NUMBER);
		// now country code
		auto it = ::identt::query::ValidIsdCodes.find(country);
		if (it==::identt::query::ValidIsdCodes.end())
			throw ::identt::query::SydentException("This country code is not supported", M_INVALID_PHONE_NUMBER);

		// check if isd code is there in phone no and matches country
		if (phone_number.substr(0,1)=="+") phone_number = phone_number.substr(1);
		else if (phone_number.substr(0,2)=="00") phone_number = phone_number.substr(2);
#ifdef IDENTT_STORE_ADD_ISD
		else {
			address=it->second + phone_number;
			return; // phone no has no country code so we added it
		}
#endif
		// the else and else if need validation ?
		if (phone_number.substr(0,it->second.length())!=it->second)
			throw ::identt::query::SydentException("Number has different country code", M_INVALID_PHONE_NUMBER);
		// phone no country code retained
		address=phone_number;
		return;
	}
	throw ::identt::query::SydentException("Invalid Email or phone no type", M_INVALID_EMAIL);
}
