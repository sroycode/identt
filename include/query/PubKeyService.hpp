/**
 * @project identt
 * @file include/query/PubKeyService.hpp
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
#ifndef _IDENTT_QUERY_PUBKEY_SERVICE_HPP_
#define _IDENTT_QUERY_PUBKEY_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/PubKeyService.hpp>

namespace identt {
namespace query {

template <class HttpServerT>
class PubKeyService :
	public identt::http::ServiceBase<HttpServerT>,
	public identt::store::PubKeyService {
public:

	/**
	* PubKeyService : constructor
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer stptr
	*
	* @param server
	*   std::shared_ptr<HttpServerT> server
	*
	* @param helpquery
	*   ::identt::query::HelpQuery::pointer helpquery
	*
	* @param scope
	*   const unsigned int scope check
	*
	* @return
	*   none
	*/

	PubKeyService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
			::identt::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: identt::http::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : GET _matrix/identity/api/v1/pubkey/{pubkey:string}
		helpquery->add({scope,
		"GET _matrix/identity/api/v1/pubkey/{pubkey:string}", {
			"This is the GET version of pubkey",
			"Required. The ID of the key. This should take the form algorithm:identifier where algorithm",
			"identifies the signing algorithm, and the identifier is an opaque string."
		}
		                    });

		server->resource["/_matrix/identity/api/v1/pubkey/([A-z0-9]*)(%3[Aa]|:)(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					PubKeyT pubkey;
					pubkey.set_algo( request->path_match[1] );
					pubkey.set_identifier( request->path_match[3] );

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					this->GetPubKeyAction(stptr, &pubkey);

					// aftermath
					std::string output;
					pb2json(&pubkey , output);
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (SydentException& e)
				{
					int ecode = (e.ecode()>=IDENTT_SYDENT_ERROR_MIN && e.ecode()<=IDENTT_SYDENT_ERROR_MAX) ? e.ecode() : M_UNKNOWN;
					std::string output = err2json(SydentErrors.at(ecode),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (identt::JdException& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (std::exception& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};

		// Endpoint : GET _matrix/identity/api/v1/pubkey/isvalid
		helpquery->add({scope,
		"GET _matrix/identity/api/v1/pubkey/isvalid?public_key=...", {
			"This is the GET version of pubkey/isvalid",
			"Check whether a long-term public key is valid.",
			"Required. The unpadded base64-encoded public key to check.",
			"Returns json with valid whether the public key is recognised and is currently valid"
		}
		                    });

		server->resource["/_matrix/identity/api/v1/pubkey/isvalid\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;

					PubKeyT pubkey;
					auto params = urldecode(request->path_match[1]);
					pubkey.set_public_key( params.at("public_key") );

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					this->GetPubKeyValidAction(stptr, &pubkey);

					// aftermath
					std::string output;
					pb2json(&pubkey , output);
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (SydentException& e)
				{
					int ecode = (e.ecode()>=IDENTT_SYDENT_ERROR_MIN && e.ecode()<=IDENTT_SYDENT_ERROR_MAX) ? e.ecode() : M_UNKNOWN;
					std::string output = err2json(SydentErrors.at(ecode),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (identt::JdException& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (std::exception& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};

		// Endpoint : GET _matrix/identity/api/v1/pubkey/ephemeral/isvalid
		helpquery->add({scope,
		"GET _matrix/identity/api/v1/pubkey/ephemeral/isvalid?public_key=...", {
			"This is the GET version of pubkey/ephemeral/isvalid",
			"Check whether a short-term public key is valid.",
			"Required. The unpadded base64-encoded public key to check.",
			"Returns json with valid whether the public key is recognised and is currently valid"
		}
		                    });

		server->resource["/_matrix/identity/api/v1/pubkey/ephemeral/isvalid\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;

					PubKeyT pubkey;
					auto params = urldecode(request->path_match[1]);
					pubkey.set_public_key( params.at("public_key") );

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					this->GetEphemeralValidAction(stptr, &pubkey);

					// aftermath
					std::string output;
					pb2json(&pubkey , output);
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (SydentException& e)
				{
					int ecode = (e.ecode()>=IDENTT_SYDENT_ERROR_MIN && e.ecode()<=IDENTT_SYDENT_ERROR_MAX) ? e.ecode() : M_UNKNOWN;
					std::string output = err2json(SydentErrors.at(ecode),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (identt::JdException& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (std::exception& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};

		// Endpoint : POST _matrix/identity/api/v1/sign-{algo:string}
		helpquery->add({scope,"POST _identt/identity/api/v1/sign-{algo:string}", {
				"The identity service will happily sign invitation details with a request-specified ed25519 private key for you",
				"params  :  mxid, token, private_key, sender (optional) ",
				"This blindly sign for client"
			}
		});

		server->resource["/_matrix/identity/api/v1/sign-([A-z0-9]*)(%3[Aa]|:)(.*)$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					identt::query::PubKeyT pubkey;
					pubkey.set_algo( request->path_match[1] );
					pubkey.set_identifier( request->path_match[3] );
					bool use_json = this->JsonRequest(request);

					identt::query::ToSignQueryT query;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , &query , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , &query); // throws on error
					}

					// validations
					if (query.mxid().length()==0)
						throw ::identt::query::SydentException("mxid value required", M_MISSING_PARAMS);
					if (query.token().length()==0)
						throw ::identt::query::SydentException("token value required", M_MISSING_PARAMS);
					if (query.private_key().length()==0)
						throw ::identt::query::SydentException("private_key value required", M_MISSING_PARAMS);

					// set things
					pubkey.set_private_key( query.private_key() );
					pubkey.set_no_keyring(true);
					if (query.sender().length()==0)
						pubkey.set_owner("unknown");
					else
						pubkey.set_owner(query.sender());

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					std::string output;
					// add signature
					::identt::query::SignedJson::SignatureListT signatures;
					AddSign(stptr, &query , &pubkey, output, signatures);

					// aftermath
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (SydentException& e)
				{
					int ecode = (e.ecode()>=IDENTT_SYDENT_ERROR_MIN && e.ecode()<=IDENTT_SYDENT_ERROR_MAX) ? e.ecode() : M_UNKNOWN;
					std::string output = err2json(SydentErrors.at(ecode),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (identt::JdException& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (std::exception& e)
				{
					std::string output = err2json(SydentErrors.at(M_UNKNOWN),e.what());
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};

	}

private:

};
} // namespace query
} // namespace identt

#endif // _IDENTT_QUERY_PUBKEY_SERVICE_HPP_
