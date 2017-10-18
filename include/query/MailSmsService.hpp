/**
 * @project identt
 * @file include/query/MailSmsService.hpp
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
 *  MailSmsService.hpp : Header for email msisdn pull service
 *
 */
#ifndef _IDENTT_QUERY_MAILSMS_SERVICE_HPP_
#define _IDENTT_QUERY_MAILSMS_SERVICE_HPP_

#include <store/MailSmsService.hpp>

namespace identt {
namespace query {

template <class HttpServerT>
class MailSmsService :
	public identt::http::ServiceBase<HttpServerT>,
	public identt::store::MailSmsService {
public:

	/**
	* MailSmsService : constructor
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

	MailSmsService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
			::identt::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: identt::http::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _identt/identity/api/v1/getmailstosend
		helpquery->add({scope,"POST _identt/identity/api/v1/getmailstosend", {
				"params  :  lastid, limit, shared_secret, payload[] ",
				"This will fetch the mail/sms that need sending.",
				"If request payload contains items marked done they will be marked deleted",
				"This is to be used only by an internal program"
			}
		});

		server->resource["/_identt/identity/api/v1/getmailstosend$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			async::parallel_invoke([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					::identt::mail::MailQueryT mailq;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , &mailq , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , &mailq); // throws on error
					}
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// check shared secret
					if (mailq.shared_secret() != stptr->shared_secret.Get())
						throw identt::BadDataException("Bad Shared Secret");

					// action
					this->PendingAction(stptr, &mailq);
					mailq.clear_shared_secret();

					// aftermath
					std::string output;
					std::vector<::identt::query::SignatureT> signatures;

					::identt::query::PubKeyT pubkey;
					// sign 
					pubkey.set_owner(stptr->hostname.Get());
					pubkey.set_algo(THREEPID_DEFAULT_ALGO);
					pubkey.set_identifier(THREEPID_DEFAULT_ALGO_ID);
					AddSign(stptr,&mailq, &pubkey, output,signatures);
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

#endif // _IDENTT_QUERY_MAILSMS_SERVICE_HPP_

