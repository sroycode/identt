/**
 * @project identt
 * @file include/query/LookupService.hpp
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
 *  LookupService.hpp : Header for email msisdn service
 *
 */
#ifndef _IDENTT_QUERY_LOOKUP_SERVICE_HPP_
#define _IDENTT_QUERY_LOOKUP_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/LookupService.hpp>

// #include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/pointer.h>

namespace identt {
namespace query {

template <class HttpServerT>
class LookupService :
	public identt::query::ServiceBase<HttpServerT> {
public:

	/**
	* LookupService : constructor
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

	LookupService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _matrix/identity/api/v1/lookup
		helpquery->add({scope,"POST _matrix/identity/api/v1/lookup", {
				"Look up the Matrix user ID for a 3pid.",
				"query parameters: medium, address",
				"medium  Required. The literal string email.",
				"address  Required. The email address being looked up."
			}
		});

		server->resource["/_matrix/identity/api/v1/lookup$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::LookupDataT lact;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , lact.mutable_query() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , lact.mutable_query() ); // throws on error
					}
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					::identt::store::LookupService lservice;
					lservice.LookupAction(stptr, &lact);

					// aftermath
					std::string output;
					::identt::query::PubKeyT pubkey;

					// set owners
					pubkey.set_owner(stptr->hostname.Get());
					pubkey.set_algo(THREEPID_DEFAULT_ALGO);
					pubkey.set_identifier(THREEPID_DEFAULT_ALGO_ID);

					// test data
					std::vector<::identt::query::SignatureT> signatures;
					lservice.AddSign(stptr, lact.mutable_result() , &pubkey, output,signatures);
					if (!output.length()) output="{}";

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

		// Endpoint : GET _matrix/identity/api/v1/lookup
		helpquery->add({scope,"GET _matrix/identity/api/v1/lookup", {
				"Look up the Matrix user ID for a 3pid.",
				"This is the GET version of lookup",
				"query parameters: medium, address",
				"medium  Required. The literal string email.",
				"address  Required. The email address being looked up."
			}
		});

		server->resource["/_matrix/identity/api/v1/lookup\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					std::string params = request->path_match[1];
					identt::query::LookupDataT lact;
					form2pb( params , lact.mutable_query()); // throws on error

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					::identt::store::LookupService lservice;
					lservice.LookupAction(stptr, &lact);

					// aftermath
					std::string output;
					::identt::query::PubKeyT pubkey;

					// set owners
					pubkey.set_owner(stptr->hostname.Get());
					pubkey.set_algo(THREEPID_DEFAULT_ALGO);
					pubkey.set_identifier(THREEPID_DEFAULT_ALGO_ID);

					// test data
					std::vector<::identt::query::SignatureT> signatures;
					lservice.AddSign(stptr, lact.mutable_result() , &pubkey, output,signatures);

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

		// Endpoint : POST _matrix/identity/api/v1/bulk_lookup
		helpquery->add({scope,"POST _matrix/identity/api/v1/bulk_lookup", {
				"Look up the Matrix user ID for a list of 3pid.",
				"query parameters: list of {medium, address}",
				"medium  Required. The literal string email.",
				"address  Required. The email address being looked up."
			}
		});

		server->resource["/_matrix/identity/api/v1/bulk_lookup$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::BulkLookupDataT blact;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , blact.mutable_query() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						throw SydentException("Only Json Supported",M_BAD_JSON);
					}
					DLOG(INFO) << blact.DebugString();
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					::identt::store::LookupService lservice;
					lservice.BulkLookupAction(stptr, &blact);
					std::string instring;
					this->BulkLookupJson(blact.mutable_result(),instring);

					// aftermath
					std::string output;
					::identt::query::PubKeyT pubkey;

					// set owners
					pubkey.set_owner(stptr->hostname.Get());
					pubkey.set_algo(THREEPID_DEFAULT_ALGO);
					pubkey.set_identifier(THREEPID_DEFAULT_ALGO_ID);


					// test data

					std::vector<::identt::query::SignatureT> signatures;
					lservice.AddSign(stptr, instring , &pubkey, output,signatures);
					if (!output.length()) output="{}";

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
	/**
	* BulkLookupJson : convert proto to json for bulk lookup
	*
	* @param bulkres
	*   identt::query::BulkLookupResultT* bulk res
	*
	* @param output
	*   std::string& output
	*
	* @return
	*   none
	*/
	void BulkLookupJson(identt::query::BulkLookupResultT* bulkres, std::string& output)
	{
		rapidjson::Document d;
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
		rapidjson::Pointer("/threepids").Create(d , allocator );
		rapidjson::Value* threepids = rapidjson::Pointer("/threepids").Get(d);
		if (!threepids->IsArray()) threepids->SetArray();

		for (auto& res : bulkres->threepids() ) {
			rapidjson::Value entry(rapidjson::kArrayType);
			entry
			.PushBack( rapidjson::StringRef( res.medium().c_str() ) , allocator )
			.PushBack( rapidjson::StringRef( res.address().c_str() ) , allocator )
			.PushBack( rapidjson::StringRef( res.mxid().c_str() ) , allocator );
			threepids->PushBack(entry,allocator);
		}
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);
		output.clear();
		output.append(buffer.GetString(), buffer.GetSize());
	}

};
} // namespace query
} // namespace identt

#endif // _IDENTT_QUERY_LOOKUP_SERVICE_HPP_
