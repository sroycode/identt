/**
 * @project identt
 * @file include/query/InfoService.hpp
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
 *  InfoService.hpp :   Info Service Implementation
 *
 */
#ifndef _IDENTT_QUERY_INFO_SERVICE_HPP_
#define _IDENTT_QUERY_INFO_SERVICE_HPP_

#include <query/QueryBase.hpp>

namespace identt {
namespace query {

template <class HttpServerT>
class InfoService : protected identt::query::ServiceBase<HttpServerT> {
public:

	/**
	* InfoService : constructor
	*
	* @param context
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

	InfoService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::query::HelpQuery::pointer helpquery,
	    const unsigned int scope)
		: identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : GET help
		helpquery->add({scope,"GET help", { "This query, Gets info about endpoints this server" } });

		server->resource["/help$"]["GET"]
		=[this,stptr,scope,helpquery](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,scope,helpquery,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string output;
					std::stringstream content_stream;
					for (auto& var: helpquery->get(scope))
					{
						content_stream << "\r\n\r\n" << var.route ;
						size_t counter=0;
						for (auto& desc: var.desc) content_stream << "\r\n -- " << desc;
					}
					output = content_stream.str();
					*response << "HTTP/1.1 200 OK\r\n";
					*response << "Content-type: text/plain\r\n";
					*response << "Content-Length: " << output.length() << "\r\n\r\n" << output.c_str();
					this->HttpOKAction(response,request,200,"OK","application/json",output,true);
				} catch (...)
				{
					this->HttpErrorAction(response,request,500,"INTERNAL SERVER ERROR");
				}
			});
		};


		// Endpoint : GET info
		helpquery->add({scope,"GET info", { "Gets info about this server" } });

		server->resource["/info$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					::identt::query::StateT sstate;
					uint64_t currtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					sstate.set_epoch( currtime );
					sstate.set_is_ready( stptr->is_ready.Get() );
					// get other params if ready
					if (sstate.is_ready())
					{
						sstate.set_hostname( stptr->hostname.Get() );
						sstate.set_master( stptr->master.Get() );
						// sstate.set_maincounter( stptr->maincounter.Get() );
						// sstate.set_logcounter( stptr->logcounter.Get() );
						sstate.set_is_master( stptr->is_master.Get() );
					}

					// aftermath
					std::string output;
					pb2json(&sstate , output);
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

#endif // _IDENTT_QUERY_INFO_SERVICE_HPP_
