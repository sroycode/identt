/**
 * @project identt
 * @file include/query/SignedJson.hpp
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
 *  SignedJson.hpp :   Json Signing Class
 *
 */
#ifndef _IDENTT_QUERY_SIGNED_JSON_HPP_
#define _IDENTT_QUERY_SIGNED_JSON_HPP_


#include <query/QueryBase.hpp>
#include <query/ProtoJsonSigned.hpp>
#include <crypto/CryptoTraits.hpp>

namespace identt {
namespace query {
class SignedJson {
public:
	using SignatureListT=std::vector<::identt::query::SignatureT>;

	/**
	* Default Constructor
	*
	*/
	SignedJson();

	/**
	* Default Destructor
	*
	*/
	virtual ~SignedJson();

	/**
	* AddSign: Add Signature pb, also adds older signatures
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param result
	*   const google::protobuf::Message* result
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param output
	*   std::string& output to populate
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void AddSign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const google::protobuf::Message* result,
	    const ::identt::query::PubKeyT* pubkey,
	    std::string& output,
	    SignatureListT& signatures
	);

	/**
	* AddSign: Add Signature pb + unsigned, also adds older signatures
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param result
	*   const google::protobuf::Message* result
	*
	* @param uresult
	*   const google::protobuf::Message* result unsigned part
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param output
	*   std::string& output to populate
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void AddSign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const google::protobuf::Message* result,
	    const google::protobuf::Message* uresult,
	    const ::identt::query::PubKeyT* pubkey,
	    std::string& output,
	    SignatureListT& signatures
	);

	/**
	* AddSign: Add Signature to canonnical json string, also adds older signatures , no unsigned
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param input
	*   const std::string input string
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param output
	*   std::string& output to populate
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void AddSign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const std::string input,
	    const ::identt::query::PubKeyT* pubkey,
	    std::string& output,
	    SignatureListT& signatures
	);

	/**
	* AddSign: Add Signature to canonnical json string, also adds older signatures , adds unsigned
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param input
	*   const std::string input string
	*
	* @param uinput
	*   const std::string input string unsigned
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param output
	*   std::string& output to populate
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void AddSign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const std::string input,
	    const std::string uinput,
	    const ::identt::query::PubKeyT* pubkey,
	    std::string& output,
	    SignatureListT& signatures
	);

	/**
	* VerifySign: Verify signature and get string
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param input
	*   const std::string input string
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param output
	*   std::string& output to populate
	*
	* @param uoutput
	*   std::string& output to populate unsigned
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void VerifySign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const std::string input,
	    const ::identt::query::PubKeyT* pubkey,
	    std::string& output,
	    std::string& uoutput,
	    SignatureListT& signatures
	);

	/**
	* VerifySign: Verify Signature pb no unsigned
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param input
	*   std::string input to get from
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param result
	*   google::protobuf::Message* result
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void VerifySign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const std::string input,
	    const ::identt::query::PubKeyT* pubkey,
	    google::protobuf::Message* result,
	    SignatureListT& signatures
	);

	/**
	* VerifySign: Verify Signature pb, with unsigned
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param input
	*   const std::string input to get from
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param result
	*   google::protobuf::Message* result
	*
	* @param uresult
	*   google::protobuf::Message* unsigned result
	*
	* @param signatures
	*   SignatureListT& signatures to append
	*
	* @return
	*   none
	*/

	void VerifySign(
	    ::identt::utils::SharedTable::pointer stptr,
	    const std::string input,
	    const ::identt::query::PubKeyT* pubkey,
	    google::protobuf::Message* result,
	    google::protobuf::Message* uresult,
	    SignatureListT& signatures
	);

private:

	/**
	* GetSigner: Get Signer pointer
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param pubkey
	*   const ::identt::query::PubKeyT* pubkey
	*
	* @param scope
	*   const unsigned int scope
	*
	* @return
	*   ::identt::crypto::CryptoTraits::pointer
	*/

	::identt::crypto::CryptoTraits::pointer GetSigner(
	    ::identt::utils::SharedTable::pointer stptr,
	    const ::identt::query::PubKeyT* pubkey,
	    const unsigned int scope
	);

};
} // namespace query
} // namespace identt
#endif /* _IDENTT_QUERY_SIGNED_JSON_HPP_ */
