/**
 * @project identt
 * @file include/query/ValidateService.hpp
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
 *  ValidateService.hpp : Header for email msisdn service
 *
 */
#ifndef _IDENTT_QUERY_VALIDATE_SERVICE_HPP_
#define _IDENTT_QUERY_VALIDATE_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/ValidateService.hpp>
#include <hrpc/HrpcClient.hpp>

namespace identt {
namespace query {

template <class HttpServerT>
class ValidateService : public identt::query::ServiceBase<HttpServerT> {
public:

	/**
	* ValidateService : constructor
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
	*
	* @param scope
	*   const unsigned int scope check
	*
	* @return
	*   none
	*/

	ValidateService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _matrix/identity/api/v1/validate/{service:string}/requestToken

		helpquery->add({scope,"POST _matrix/identity/api/v1/validate/{service:string}/requestToken", {
				"service can be email or msisdn ",
				"params  : email , client_secret , send_attempt, next_link",
				"params  : phone_number, country , client_secret , send_attempt, next_link",
				"This will create a new session on the identity service, identified by an sid.",
				"It may also optionally specify next_link to redirect on successful validation.",
				"The identity service will send an email/sms containing a token to verify.",
				"We return the sid generated for this session to the caller.",
				"If a send_attempt is specified, the server will only send an request if the ",
				"send_attempt is a number greater than the most recent one which it has seen",
				"Note that Home Servers offer APIs that proxy this API"
			}
		});

		server->resource["/_matrix/identity/api/v1/validate/(email|msisdn)/requestToken$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					identt::query::RequestTokenDataT rtoka;
					rtoka.set_medium ( request->path_match[1] );
					bool use_json = this->JsonRequest(request);

					if (use_json)
					{
						int stat = json2pb( request->content.string() , rtoka.mutable_reqtok() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , rtoka.mutable_reqtok()); // throws on error
					}
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::identt::store::ValidateService valservice;
						valservice.RequestTokenAction(stptr, &rtoka);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_REQUESTTOKEN,&rtoka);
					}

					// aftermath
					std::string output;
					pb2json(rtoka.mutable_ssid() , output);
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

		// Endpoint : GET _matrix/identity/api/v1/validate/{service:string}/requestToken

		helpquery->add({scope, "GET _matrix/identity/api/v1/validate/{service:string}/requestToken?{params}", {
				"service can be email or msisdn ",
				"params  : email , client_secret , send_attempt, next_link",
				"params  : phone_number, country , client_secret , send_attempt, next_link",
				"This is the GET version of requestToken"
				"Note that Home Servers offer APIs that proxy this API"
			}
		});

		server->resource["/_matrix/identity/api/v1/validate/(email|msisdn)/requestToken\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					identt::query::RequestTokenDataT rtoka;
					rtoka.set_medium ( request->path_match[1] );

					std::string params = request->path_match[2];
					form2pb( params , rtoka.mutable_reqtok()); // throws on error

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::identt::store::ValidateService valservice;
						valservice.RequestTokenAction(stptr, &rtoka);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_REQUESTTOKEN,&rtoka);
					}

					// aftermath
					std::string output;
					pb2json(rtoka.mutable_ssid() , output);
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

		// Endpoint : POST _matrix/identity/api/v1/validate/{service:string}/submitToken

		helpquery->add({scope,"POST _matrix/identity/api/v1/validate/{service:string}/submitToken", {
				"service can be email or msisdn ",
				"A  user may make a POST request with the following parameters",
				"sid: the sid for the  session,  generated  by  the  requestToken call.",
				"client_secret: the client secret which was supplied to the requestToken call.",
				"token: the token generated by the  requestToken call, and emailed to the user.",
				"If  these  three  values are consistent with a set generated by a requestToken call,",
				"ownership of the email address  is  considered to  have  been  validated.",
				"Note that Home Servers offer APIs that proxy this API"
			}
		});

		server->resource["/_matrix/identity/api/v1/validate/(email|msisdn)/submitToken$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					identt::query::SubmitTokenDataT stoka;
					stoka.set_medium ( request->path_match[1] );
					bool use_json = this->JsonRequest(request);

					if (use_json)
					{
						int stat = json2pb( request->content.string() , stoka.mutable_subtok() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , stoka.mutable_subtok()); // throws on error
					}
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::identt::store::ValidateService valservice;
						valservice.SubmitTokenAction(stptr, &stoka);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_SUBMITTOKEN,&stoka);
					}

					// aftermath
					std::string output;
					pb2json(stoka.mutable_result() , output);
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

		// Endpoint : GET _matrix/identity/api/v1/validate/{service:string}/submitToken

		helpquery->add({scope,"GET _matrix/identity/api/v1/validate/{service:string}/submitToken?{params}", {
				"service can be email or msisdn ",
				"params  : sid , client_secret , token ",
				"This is the GET version of submitToken"
				"Note that Home Servers offer APIs that proxy this API"
			}
		});

		server->resource["/_matrix/identity/api/v1/validate/(email|msisdn)/submitToken\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					identt::query::SubmitTokenDataT stoka;
					stoka.set_medium ( request->path_match[1] );
					std::string params = request->path_match[2];

					form2pb( params , stoka.mutable_subtok()); // throws on error
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get()) {
						::identt::store::ValidateService valservice;
						valservice.SubmitTokenAction(stptr, &stoka);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_SUBMITTOKEN,&stoka);
					}

					// aftermath
					std::string output;
					pb2json(stoka.mutable_result() , output);
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

#endif // _IDENTT_QUERY_VALIDATE_SERVICE_HPP_

