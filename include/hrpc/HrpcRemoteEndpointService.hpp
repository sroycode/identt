/**
 * @project identt
 * @file include/hrpc/HrpcRemoteEndpointService.hpp
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
 *  HrpcRemoteEndpointService.hpp : Header remote only services
 *
 */
#ifndef _IDENTT_HRPC_REMOTE_ENDPOINT_SERVICE_HPP_
#define _IDENTT_HRPC_REMOTE_ENDPOINT_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <hrpc/ProtoServiceBase.hpp>
#include <store/StoreTrans.hpp>

#include <hrpc/RemoteKeeper.hpp>

namespace identt {
namespace hrpc {

template <class HttpServerT>
class HrpcRemoteEndpointService :
	public identt::query::ServiceBase<HttpServerT>,
	protected identt::hrpc::ProtoServiceBase<HttpServerT> {
public:

	/**
	* HrpcRemoteEndpointService : constructor
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

	HrpcRemoteEndpointService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::hrpc::RemoteKeeper::pointer rkeeper,
	    unsigned int scope)
		:
		identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _identt/identity/remote/v1/endpoint

		server->resource["/_identt/identity/remote/v1/endpoint$"]["POST"]
		=[this,stptr,rkeeper](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				std::string output;
				int ecode=M_UNKNOWN;
				try {
					if (!this->ProtoRequest(request))
						throw identt::BadDataException("Only Protobuf Supported");
					if (!this->SharedSecretOK(request,stptr->shared_secret.Get()))
						throw identt::BadDataException("Bad Shared Secret");

					auto sname = this->ServiceName(request);

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// start here
					switch(sname)
					{
					case ::identt::hrpc::R_GETINFO : {
						// action
						::identt::hrpc::StateT sstate;
						sstate.set_ts( IDENTT_CURRTIME_MS );
						sstate.set_is_ready( stptr->is_ready.Get() );
						// get other params if ready
						sstate.set_hostname( stptr->hostname.Get() );
						sstate.set_master( stptr->master.Get() );
						sstate.set_maincounter( stptr->maincounter.Get() );
						sstate.set_logcounter( stptr->logcounter.Get() );
						sstate.set_is_master( stptr->is_master.Get() );
						sstate.set_thisurl( stptr->thisurl.Get() );
						sstate.SerializeToString(&output);
						LOG(INFO) << request->path;
						break;
					}
					case ::identt::hrpc::R_READONE : {
						identt::store::TransactionT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::StoreTrans service; //ReadOne
						service.ReadOne(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
						break;
					}
					case ::identt::hrpc::R_TRANSLOG : {
						break;
						identt::store::TransListT data;
						if (!data.ParseFromString( request->content.string() ))
							throw identt::BadDataException("Bad Protobuf Format");
						// action
						identt::store::StoreTrans service; //ReadLog
						service.ReadLog(stptr, &data);
						// aftermath
						data.SerializeToString(&output);
					}
					default:
						LOG(INFO) << request->path;
						throw identt::BadDataException("Service Name unknown or unimplemented");
						break;
					}
					ecode=0; // something reached here
					// end here

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

#endif // _IDENTT_HRPC_REMOTE_ENDPOINT_SERVICE_HPP_

