/**
 * @project identt
 * @file include/query/ThreePidService.hpp
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
 *  ThreePidService.hpp : Header for email msisdn service
 *
 */
#ifndef _IDENTT_QUERY_THREEPID_SERVICE_HPP_
#define _IDENTT_QUERY_THREEPID_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/ThreePidService.hpp>

namespace identt {
namespace query {

template <class HttpServerT>
class ThreePidService : public identt::http::ServiceBase<HttpServerT>,
	identt::store::ThreePidService {
public:

	/**
	* ThreePidService : constructor
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer stptr
	*
	* @param server
	*   HttpServerT server
	*
	* @param scope
	*   const unsigned int scope check
	*
	* @return
	*   none
	*/

	ThreePidService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    unsigned int scope)
		: identt::http::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _matrix/identity/api/v1/3pid/getValidated3pid
		stptr->httphelp.add({scope,"POST _matrix/identity/api/v1/3pid/getValidated3pid", {
				"A client can check whether ownership of a 3pid was validated by making a",
				" request passing the sid and client_secret as query parameters from the requestToken call",
				"params  : sid , client_secret"
			}
		});
		server->resource["/_matrix/identity/api/v1/3pid/getValidated3pid$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::GetValidated3pidDataT gva;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , gva.mutable_subtok() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , gva.mutable_subtok()); // throws on error
					}
					if (!stptr->is_ready()) throw identt::BadDataException("System Not Ready");
					identt::query::ValidatedAtT valresult;
					// action
					this->GetValidated3pidAction(stptr, &gva);

					// aftermath
					std::string output;
					pb2json(gva.mutable_valresult() , output);
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

		// Endpoint : GET _matrix/identity/api/v1/3pid/getValidated3pid
		stptr->httphelp.add({scope,"GET _matrix/identity/api/v1/3pid/getValidated3pid", {
				"This is the GET version of getValidated3pid",
				"A client can check whether ownership of a 3pid was validated by making a",
				" request passing the sid and client_secret as query parameters from the requestToken call",
				"params  : sid , client_secret"
			}
		});

		server->resource["/_matrix/identity/api/v1/3pid/getValidated3pid\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					std::string params = request->path_match[1];

					identt::query::GetValidated3pidDataT gva;
					form2pb( params , gva.mutable_subtok()); // throws on error
					if (!stptr->is_ready()) throw identt::BadDataException("System Not Ready");

					// action
					this->GetValidated3pidAction(stptr, &gva);

					// aftermath
					std::string output;
					pb2json(gva.mutable_valresult() , output);
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

		// Endpoint : POST _matrix/identity/api/v1/3pid/bind
		stptr->httphelp.add({scope,"POST _matrix/identity/api/v1/3pid/bind", {
				"An association between a session and a Matrix user ID can be published by  making  a request to this.",
				"params: sid , client_secret , mxid",
				"where mxid takes the form @name:domain.com",
				"If  the  session is still valid, this will publish an association between the 3pids validated on that",
				"session and the passed Matrix user  ID.  Future calls to /lookup for any of the session’s 3pids",
				"will return this association.  If the 3pid has not yet been validated, the HTTP request will  be",
				"rejected, and the association will not be established.  If  the  sid  and  client_secret were not recognised",
				"or were not correct, an error will be returned."
			}
		});

		server->resource["/_matrix/identity/api/v1/3pid/bind$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::Bind3pidDataT bpa;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , bpa.mutable_subtok() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , bpa.mutable_subtok()); // throws on error
					}
					if (!stptr->is_ready()) throw identt::BadDataException("System Not Ready");
					// action
					this->Bind3pidAction(stptr, &bpa);
					// copy the string , change this later - shreos
					std::string output = bpa.output();

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
		// Endpoint : GET _matrix/identity/api/v1/3pid/bind
		stptr->httphelp.add({scope,"GET _matrix/identity/api/v1/3pid/bind", {
				"This is the GET version of 3pid/bind",
				"An association between a session and a Matrix user ID can be published by  making  a request to this.",
				"params: sid , client_secret , mxid",
				"where mxid takes the form @name:domain.com"
			}
		});

		server->resource["/_matrix/identity/api/v1/3pid/bind\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					std::string params = request->path_match[1];

					identt::query::Bind3pidDataT bpa;
					form2pb( params , bpa.mutable_subtok()); // throws on error
					if (!stptr->is_ready()) throw identt::BadDataException("System Not Ready");

					// action
					this->Bind3pidAction(stptr, &bpa);
					std::string output = bpa.output();

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

#endif // _IDENTT_QUERY_THREEPID_SERVICE_HPP_

