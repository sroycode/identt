/**
 * @project identt
 * @file include/hrpc/HrpcMasterEndpointService.hpp
 * @author  S Roychowdhury
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2018 S Roychowdhury
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
 *  HrpcMasterEndpointService.hpp : Header master only services
 *
 */
#ifndef _IDENTT_HRPC_MASTER_ENDPOINT_SERVICE_HPP_
#define _IDENTT_HRPC_MASTER_ENDPOINT_SERVICE_HPP_


#include "query/QueryBase.hpp"
#include "hrpc/ProtoServiceBase.hpp"
#include "store/InviteService.hpp"
#include "store/LookupService.hpp"
#include "store/MailSmsService.hpp"
#include "store/PubKeyService.hpp"
#include "store/ThreePidService.hpp"
#include "store/ValidateService.hpp"
#include "store/BlobDataService.hpp"
#include "store/StoreTrans.hpp"
#include "hrpc/RemoteKeeper.hpp"

namespace identt {
namespace hrpc {

template <class HttpServerT>
class HrpcMasterEndpointService :
	public identt::query::ServiceBase<HttpServerT>,
	protected identt::hrpc::ProtoServiceBase<HttpServerT> {
public:

	/**
	* HrpcMasterEndpointService : constructor
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer stptr
	*
	* @param server
	*   std::shared_ptr<HttpServerT> server
	*
	* @param rkeeper
	*   ::identt::hrpc::RemoteKeeper::pointer rkeeper
	*
	* @param scope
	*   const unsigned int scope check
	*
	* @return
	*   none
	*/

	HrpcMasterEndpointService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::hrpc::RemoteKeeper::pointer rkeeper,
	    unsigned int scope)
		:
		identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _identt/identity/master/v1/endpoint

		server->resource["/_identt/identity/master/v1/endpoint$"]["POST"]
		=[this,stptr,rkeeper](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				std::string output;
				int ecode=M_UNKNOWN;
				try
				{
					if (!this->ProtoRequest(request))
						throw identt::BadDataException("Only Protobuf Supported");
					if (!this->SharedSecretOK(request,stptr->shared_secret.Get()))
						throw identt::BadDataException("Bad Shared Secret");

					auto sname = this->ServiceName(request);

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");
					if (!stptr->is_master.Get()) throw identt::BadDataException("System Not Master");

					// start here
					switch(sname) {
					default: {
						DLOG(INFO) << request->path;
						throw identt::BadDataException("Service Name unknown or unimplemented");
						break;
					}
					case ::identt::hrpc::M_REQUESTTOKEN : {
						identt::query::RequestTokenDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ValidateService service; //RequestTokenAction
						service.RequestTokenAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_SUBMITTOKEN : {
						identt::query::SubmitTokenDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ValidateService service; //SubmitTokenAction
						service.SubmitTokenAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_GETVALIDATED3PID : {
						identt::query::GetValidated3pidDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ThreePidService service; //GetValidated3pidAction
						service.GetValidated3pidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_BIND3PID : {
						identt::query::Bind3pidDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::ThreePidService service; //Bind3pidAction
						service.Bind3pidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_LOOKUP : {
						identt::query::LookupDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::LookupService service; //LookupAction
						service.LookupAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_BULKLOOKUP : {
						identt::query::BulkLookupDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::LookupService service; //BulkLookupAction
						service.BulkLookupAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_GETPUBKEY : {
						identt::query::PubKeyT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::PubKeyService service; //GetPubKeyAction
						service.GetPubKeyAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_GETPUBKEYVALID : {
						identt::query::PubKeyT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::PubKeyService service; //GetPubKeyValidAction
						service.GetPubKeyValidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_GETEPHEMERALVALID : {
						identt::query::PubKeyT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::PubKeyService service; //GetEphemeralValidAction
						service.GetEphemeralValidAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_STOREINVITE : {
						identt::query::StoreInviteDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::InviteService service; //StoreInviteAction
						service.StoreInviteAction(stptr,&data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_PENDING : {
						identt::mail::MailQueryT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::MailSmsService service; //PendingAction
						service.PendingAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_GETACCESSKEY : {
						identt::query::GetAccessDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::BlobDataService service; // GetAccessKeyAction
						service.GetAccessKeyAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_SETBLOBDATA : {
						identt::query::SetBlobDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::BlobDataService service; // GetBlobDataAction
						service.SetBlobDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::M_GETBLOBDATA : {
						identt::query::GetBlobDataT data;
						if (!data.ParseFromString( b64_decode(request->content.string()) ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::BlobDataService service; // GetBlobDataAction
						service.GetBlobDataAction(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						DLOG(INFO) << request->path;
						break;
					}
					}
					// end here
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

				if (ecode==0)
				{
					this->ServiceOKAction(response,request,b64_encode(output));
				}
				else
				{
					this->ServiceErrAction(response,request,ecode,output);
				}
			});
		};
	}
};
} // namespace query
} // namespace identt

#endif // _IDENTT_HRPC_MASTER_ENDPOINT_SERVICE_HPP_

