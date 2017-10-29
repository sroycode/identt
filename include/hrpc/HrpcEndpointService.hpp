/**
 * @project identt
 * @file include/hrpc/HrpcEndpointService.hpp
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
 *  HrpcEndpointService.hpp : Header master only services
 *
 */
#ifndef _IDENTT_HRPC_ENDPOINT_SERVICE_HPP_
#define _IDENTT_HRPC_ENDPOINT_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <hrpc/ProtoServiceBase.hpp>
#include <store/InviteService.hpp>
#include <store/LookupService.hpp>
#include <store/MailSmsService.hpp>
#include <store/PubKeyService.hpp>
#include <store/ThreePidService.hpp>
#include <store/ValidateService.hpp>
#include <store/StoreTrans.hpp>

namespace identt {
namespace hrpc {

template <class HttpServerT>
class HrpcEndpointService :
	public identt::query::ServiceBase<HttpServerT>,
	protected identt::hrpc::ProtoServiceBase<HttpServerT> {
public:

	/**
	* HrpcEndpointService : constructor
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

	HrpcEndpointService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		:
		identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _identt/identity/master/v1/endpoint
		helpquery->add({scope,"POST _identt/identity/master/v1/endpoint", {
				"This is for master endpoints only between servers",
				"Uses protobuf data format only hence not documented here."
			}
		});

		server->resource["/_identt/identity/master/v1/endpoint$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				std::string output;
				int ecode=M_UNKNOWN;
				try {
					if (!this->ProtoRequest(request))
						throw identt::BadDataException("Only Protobuf Supported");
					if (!this->SharedSecretOK(request,stptr->shared_secret.Get()))
						throw identt::BadDataException("Bad Shared Secret");

					std::string sname = this->ServiceName(request);
					if (sname.empty())
						throw identt::BadDataException("Service-Name not defined");


					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");
					if (!stptr->is_master.Get()) throw identt::BadDataException("System Not Master");


					if (sname=="info")
					{
						// action
						::identt::query::StateT sstate;
						uint64_t currtime
							= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						sstate.set_epoch( currtime );
						sstate.set_is_ready( stptr->is_ready.Get() );
						// get other params if ready
						if (sstate.is_ready())
						{
							sstate.set_hostname( stptr->hostname.Get() );
							sstate.set_master( stptr->master.Get() );
							sstate.set_maincounter( stptr->maincounter.Get() );
							sstate.set_logcounter( stptr->logcounter.Get() );
							sstate.set_is_master( stptr->is_master.Get() );
						}
						sstate.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="TransLog")
					{
						identt::store::TransListT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::StoreTrans service; //ReadLog
						service.ReadLog(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
					} else if (sname=="RequestToken")
					{
						identt::query::RequestTokenDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ValidateService service; //RequestTokenAction
						service.RequestTokenAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="SubmitToken")
					{
						identt::query::SubmitTokenDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ValidateService service; //SubmitTokenAction
						service.SubmitTokenAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="GetValidated3pid")
					{
						identt::query::GetValidated3pidDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ThreePidService service; //GetValidated3pidAction
						service.GetValidated3pidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="Bind3pid")
					{
						identt::query::Bind3pidDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ThreePidService service; //Bind3pidAction
						service.Bind3pidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="Lookup")
					{
						identt::query::LookupDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::LookupService service; //LookupAction
						service.LookupAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="BulkLookup")
					{
						identt::query::BulkLookupDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::LookupService service; //BulkLookupAction
						service.BulkLookupAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="GetPubKey")
					{
						identt::query::PubKeyT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::PubKeyService service; //GetPubKeyAction
						service.GetPubKeyAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="GetPubKeyValid")
					{
						identt::query::PubKeyT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::PubKeyService service; //GetPubKeyValidAction
						service.GetPubKeyValidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="GetEphemeralValid")
					{
						identt::query::PubKeyT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::PubKeyService service; //GetEphemeralValidAction
						service.GetEphemeralValidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="StoreInvite")
					{
						identt::query::StoreInviteDataT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::InviteService service; //StoreInviteAction
						service.StoreInviteAction(stptr,&data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else if (sname=="Pending")
					{
						identt::mail::MailQueryT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::MailSmsService service; //PendingAction
						service.PendingAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						LOG(INFO) << request->path;
					} else {
						throw identt::BadDataException("Service Name unknown or unimplemented");
					}
					ecode=0; // something reached here

				} catch (::identt::query::SydentException& e)
				{
					int ecode = (e.ecode()>=IDENTT_SYDENT_ERROR_MIN && e.ecode()<=IDENTT_SYDENT_ERROR_MAX) ? e.ecode() : M_UNKNOWN;
					output=e.what();
				} catch (identt::JdException& e)
				{
					ecode=M_UNKNOWN; // default
					output=e.what();
				} catch (std::exception& e)
				{
					ecode=M_UNKNOWN; // default
					output=e.what();
				} catch (...)
				{
					ecode=M_UNKNOWN; // default
					output="Unknown Error";
				}
				if (ecode==0) this->ServiceOKAction(response,request,output);
				else this->ServiceErrAction(response,request,ecode,output);
			});
		};
	}
};
} // namespace query
} // namespace identt

#endif // _IDENTT_HRPC_ENDPOINT_SERVICE_HPP_

