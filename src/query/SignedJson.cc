/**
 * @project identt
 * @file src/query/SignedJson.cc
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
 *  SignedJson.cc :   Json Signing Class Implementation
 *
 */

#include <query/SignedJson.hpp>
#include <crypto/CryptoTraits.hpp>

/**
* Default Constructor
*
*/
identt::query::SignedJson::SignedJson() {}

/**
* Default Destructor
*
*/
identt::query::SignedJson::~SignedJson() {}

/**
* GetSigner: Get Signer pointer
*
*/

::identt::crypto::CryptoTraits::pointer identt::query::SignedJson::GetSigner(
    ::identt::utils::SharedTable::pointer stptr,const ::identt::query::PubKeyT* pubkey, const unsigned int scope)
{
	std::string pkid = pubkey->algo() + std::string(":") + pubkey->identifier();
	if (!pubkey->no_keyring()) {
		if (stptr->keyring.find(pkid) == stptr->keyring.end())
			throw ::identt::query::SydentException("Invalid Signing Key",M_BAD_JSON);
		return stptr->keyring[pkid];
	}
	if (scope & IDENTT_CRYPTO_SCOPE_SIGNIT_VERIFY) {
		return ::identt::crypto::CryptoTraits::Create(pubkey->algo(), pubkey->public_key(), pubkey->private_key());
	} else if (scope & IDENTT_CRYPTO_SCOPE_SIGNIT) {
		return ::identt::crypto::CryptoTraits::Create(pubkey->algo(), IDENTT_CRYPTO_SCOPE_SIGNIT, pubkey->private_key());
	} else if (scope & IDENTT_CRYPTO_SCOPE_VERIFY) {
		return ::identt::crypto::CryptoTraits::Create(pubkey->algo(), IDENTT_CRYPTO_SCOPE_VERIFY, pubkey->public_key());
	}
	throw ::identt::query::SydentException("Invalid scope of Signing Key",M_BAD_JSON);
}

/**
* AddSign: Add Signature pb, also adds older signatures
*
*/

void identt::query::SignedJson::AddSign(
    ::identt::utils::SharedTable::pointer stptr,
    const google::protobuf::Message* result,
    const ::identt::query::PubKeyT* pubkey,
    std::string& output,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{
	::identt::crypto::CryptoTraits::pointer mykey = GetSigner(stptr,pubkey,IDENTT_CRYPTO_SCOPE_SIGNIT);
	std::string tosign;
	std::string pkid = pubkey->algo() + std::string(":") + pubkey->identifier();
	::identt::query::pb2json(result,tosign);
	signatures.push_back({ pubkey->owner(),pkid,mykey->GetSignature(tosign) });
	::identt::query::pb2signedjson(result,output,signatures);
}

/**
* AddSign: Add Signature pb with unsigned, also adds older signatures
*
*/

void identt::query::SignedJson::AddSign(
    ::identt::utils::SharedTable::pointer stptr,
    const google::protobuf::Message* result,
    const google::protobuf::Message* uresult,
    const ::identt::query::PubKeyT* pubkey,
    std::string& output,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{
	::identt::crypto::CryptoTraits::pointer mykey = GetSigner(stptr,pubkey,IDENTT_CRYPTO_SCOPE_SIGNIT);
	std::string pkid = pubkey->algo() + std::string(":") + pubkey->identifier();
	std::string tosign;
	::identt::query::pb2json(result,tosign);
	signatures.push_back({ pubkey->owner(),pkid,mykey->GetSignature(tosign) });
	::identt::query::pb2signedjson(result,uresult,output,signatures);
}

/**
* AddSign: Add Signature to canonnical json string, also adds older signatures, no unsigned
*
*/

void identt::query::SignedJson::AddSign(
    ::identt::utils::SharedTable::pointer stptr,
    const std::string input,
    const ::identt::query::PubKeyT* pubkey,
    std::string& output,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{
	::identt::crypto::CryptoTraits::pointer mykey = GetSigner(stptr,pubkey,IDENTT_CRYPTO_SCOPE_SIGNIT);
	std::string pkid = pubkey->algo() + std::string(":") + pubkey->identifier();
	signatures.push_back({ pubkey->owner(),pkid,mykey->GetSignature(input) });
	std::string err,uinput;
	int ret = identt::query::unsigned2signedjson(input,uinput,output,err,signatures);
	if (ret<0) throw ::identt::query::SydentException("Bad Json Format: "+err,M_BAD_JSON);
}

/**
* AddSign: Add Signature to canonnical json string, also adds older signatures w unsigned
*
*/

void identt::query::SignedJson::AddSign(
    ::identt::utils::SharedTable::pointer stptr,
    const std::string input,
    const std::string uinput,
    const ::identt::query::PubKeyT* pubkey,
    std::string& output,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{
	::identt::crypto::CryptoTraits::pointer mykey = GetSigner(stptr,pubkey,IDENTT_CRYPTO_SCOPE_SIGNIT);
	std::string pkid = pubkey->algo() + std::string(":") + pubkey->identifier();
	signatures.push_back({ pubkey->owner(),pkid,mykey->GetSignature(input) });
	std::string err;
	int ret = identt::query::unsigned2signedjson(input,uinput,output,err,signatures);
	if (ret<0) throw ::identt::query::SydentException("Bad Json Format: "+err,M_BAD_JSON);
}

/**
* VerifySign: Verify signature and get string with unsigned
*
*/

void identt::query::SignedJson::VerifySign(
    ::identt::utils::SharedTable::pointer stptr,
    const std::string input,
    const ::identt::query::PubKeyT* pubkey,
    std::string& output,
    std::string& uoutput,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{

	std::string err,uns;
	int stat = signedjson2unsigned( input , output, uoutput, err, signatures);
	if (stat<0)
		throw ::identt::query::SydentException("Invalid Json cannot create unsigned",M_BAD_JSON);
	std::string pkid = pubkey->algo() + std::string(":") + pubkey->identifier();
	bool signature_found=false;
	std::string signa;
	for (auto& s : signatures) {
		if (s.org==pubkey->owner() && s.key==pkid) {
			signa=s.value;
			signature_found=true;
		}
	}
	if (!signature_found)
		throw ::identt::query::SydentException("Invalid Json no signature",M_BAD_JSON);

	::identt::crypto::CryptoTraits::pointer mykey = GetSigner(stptr,pubkey,IDENTT_CRYPTO_SCOPE_VERIFY);
	if (!mykey->VerifySignature(signa,output))
		throw ::identt::query::SydentException("Invalid Signature",M_BAD_JSON);
}

/**
* VerifySign: Verify signature and get pb wo unsigned
*
*/

void identt::query::SignedJson::VerifySign(
    ::identt::utils::SharedTable::pointer stptr,
    const std::string input,
    const ::identt::query::PubKeyT* pubkey,
    google::protobuf::Message* result,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{
	std::string output,uoutput,err;
	this->VerifySign(stptr,input,pubkey,output,uoutput,signatures);
	int stat = json2pb(output , result, err);
	if (stat<0)
		throw ::identt::query::SydentException("Invalid Json cannot make proto",M_BAD_JSON);
}

/**
* VerifySign: Verify signature and get pb w unsigned
*
*/

void identt::query::SignedJson::VerifySign(
    ::identt::utils::SharedTable::pointer stptr,
    const std::string input,
    const ::identt::query::PubKeyT* pubkey,
    google::protobuf::Message* result,
    google::protobuf::Message* uresult,
    ::identt::query::SignedJson::SignatureListT& signatures
)
{
	std::string output,uoutput,err;
	this->VerifySign(stptr,input,pubkey,output,uoutput,signatures);
	int stat = json2pb(output , result, err);
	if (stat<0)
		throw ::identt::query::SydentException("Invalid Json cannot make proto",M_BAD_JSON);
	if (uoutput.length()==0) return;
	stat = json2pb(uoutput , uresult, err);
	if (stat<0)
		throw ::identt::query::SydentException("Invalid Json cannot make proto from unsigned",M_BAD_JSON);
}

