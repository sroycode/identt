/**
 * @project identt
 * @file include/query/InviteService.hpp
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
 *  InviteService.hpp : Header for email msisdn service
 *
 */
#ifndef _IDENTT_QUERY_INVITE_SERVICE_HPP_
#define _IDENTT_QUERY_INVITE_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/InviteService.hpp>

namespace identt {
namespace query {

template <class HttpServerT>
class InviteService : public identt::http::ServiceBase<HttpServerT>,
	identt::store::InviteService {
public:

	/**
	* InviteService : constructor
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

	InviteService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    unsigned int scope)
		: identt::http::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _matrix/identity/api/v1/store-invite
		stptr->httphelp.add({scope,"POST _matrix/identity/api/v1/store-invite", {
				"An  identity  service can store pending invitations to a user’s 3pid, which will",
				"be retrieved and can be either notified on or look up when the 3pid is associated with a Matrix user ID.",
				"following URL‐encoded POST parameters:",
				" ‐ medium (string, required): The literal string email.",
				" ‐ address (string, required): The email address or msisdn of the invited user.",
				" ‐ room_id (string, required): The Matrix room ID to which the user is  invited.",
				" ‐ sender (string, required): The matrix user ID of the inviting user.",
				"An arbitrary number of other parameters may also be specified."
			}
		});

		server->resource["/_matrix/identity/api/v1/store-invite$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::StoreInviteDataT inv;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , inv.mutable_invqry() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						throw SydentException("Content-Type json only allowed",M_BAD_JSON);
					}
					if (!stptr->is_ready()) throw identt::BadDataException("System Not Ready");

					// action
					this->StoreInviteAction(stptr,&inv);

					// aftermath
					std::string output;
					pb2json(inv.mutable_invres() , output);
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

#endif // _IDENTT_QUERY_INVITE_SERVICE_HPP_

