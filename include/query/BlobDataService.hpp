/**
 * @project identt
 * @file include/query/BlobDataService.hpp
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
 *  BlobDataService.hpp : Header for accessing blobdata service
 *
 */
#ifndef _IDENTT_QUERY_BLOBDATA_SERVICE_HPP_
#define _IDENTT_QUERY_BLOBDATA_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/BlobDataService.hpp>
#include <hrpc/HrpcClient.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/pointer.h>

namespace identt {
namespace query {

template <class HttpServerT>
class BlobDataService : public identt::query::ServiceBase<HttpServerT> {
public:

	/**
	* BlobDataService : constructor
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

	BlobDataService(
	    identt::utils::SharedTable::pointer stptr,
	    typename std::shared_ptr<HttpServerT> server,
	    ::identt::query::HelpQuery::pointer helpquery,
	    unsigned int scope)
		: identt::query::ServiceBase<HttpServerT>(IDENTT_SERVICE_SCOPE_HTTP | IDENTT_SERVICE_SCOPE_HTTPS)
	{
		if (!(this->myscope & scope)) return; // scope mismatch

		// Endpoint : POST _identt/identity/api/v1/blobdata/getAccessKey
		helpquery->add({scope,"POST _identt/identity/api/v1/blobdata/getAccessKey", {
				"This is the POST version of getAccessKey",
				"A client can get access key for storing arbitrary json data by getting an access key",
				"params  : sid , client_secret",
				"returns the access key if ok"
			}
		});
		server->resource["/_identt/identity/api/v1/blobdata/getAccessKey$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::GetAccessDataT gva;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , gva.mutable_subtok() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , gva.mutable_subtok()); // throws on error
					}
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					if (gva.mutable_subtok()->sid().length()==0)
						throw ::identt::query::SydentException("sid value required", M_MISSING_PARAMS);
					if (gva.mutable_subtok()->client_secret().length()==0)
						throw ::identt::query::SydentException("client_secret value required", M_MISSING_PARAMS);

					if (stptr->is_master.Get())
					{
						::identt::store::BlobDataService bservice;
						bservice.GetAccessKeyAction(stptr, &gva);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_GETACCESSKEY,&gva);
					}

					// aftermath
					std::string output;
					pb2json(gva.mutable_accessblob() , output);
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

		// Endpoint : GET _identt/identity/api/v1/blobdata/getAccessKey
		helpquery->add({scope,"GET _identt/identity/api/v1/blobdata/getAccessKey", {
				"This is the GET version of getAccessKey",
				"A client can get access key for storing arbitrary json data by getting an access key",
				"params  : sid , client_secret",
				"returns the access key if ok"
			}
		});

		server->resource["/_identt/identity/api/v1/blobdata/getAccessKey\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					std::string params = request->path_match[1];

					identt::query::GetAccessDataT gva;
					form2pb( params , gva.mutable_subtok()); // throws on error
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					if (stptr->is_master.Get())
					{
						::identt::store::BlobDataService bservice;
						bservice.GetAccessKeyAction(stptr, &gva);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_GETACCESSKEY,&gva);
					}

					// aftermath
					std::string output;
					pb2json(gva.mutable_accessblob() , output);
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

		// Endpoint : POST _identt/identity/api/v1/blobdata/getBlobData
		helpquery->add({scope,"POST _identt/identity/api/v1/blobdata/getBlobData", {
				"This is the POST version of getBlobData",
				"A client can store arbitrary json data by access key and tag",
				"params  : accesskey, tag",
				"returns the jsondata and textdata  if ok"
			}
		});
		server->resource["/_identt/identity/api/v1/blobdata/getBlobData$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);

					identt::query::GetBlobDataT bdata;
					if (use_json)
					{
						int stat = json2pb( request->content.string() , bdata.mutable_accessblob() , err);
						if (stat<0) throw SydentException("Bad Json Format",M_BAD_JSON);
					} else {
						form2pb( request->content.string() , bdata.mutable_accessblob()); // throws on error
					}
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action

					::identt::store::BlobDataService bservice;
					bservice.GetBlobDataAction(stptr, &bdata);

					// aftermath
					if ( bdata.mutable_accessblob()->accessdenied() ) throw identt::BadDataException("Access Denied");
					std::string output;
					this->HandleDataOut(bdata.mutable_accessblob() , output);

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

		// Endpoint : GET _identt/identity/api/v1/blobdata/getBlobData
		helpquery->add({scope,"GET _identt/identity/api/v1/blobdata/getBlobData", {
				"This is the GET version of getBlobData",
				"A client can store arbitrary json data by access key and tag",
				"params  : accesskey, tag",
				"returns the jsondata and textdata  if ok"
			}
		});

		server->resource["/_identt/identity/api/v1/blobdata/getBlobData\\\?(.*)$"]["GET"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					std::string params = request->path_match[1];

					identt::query::GetBlobDataT bdata;
					form2pb( params , bdata.mutable_accessblob()); // throws on error
					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					// action
					::identt::store::BlobDataService bservice;
					bservice.GetBlobDataAction(stptr, &bdata);

					// aftermath
					if ( bdata.mutable_accessblob()->accessdenied() ) throw identt::BadDataException("Access Denied");
					std::string output;
					this->HandleDataOut(bdata.mutable_accessblob() , output);

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

		// Endpoint : POST _identt/identity/api/v1/blobdata/setBlobData
		helpquery->add({scope,"POST _identt/identity/api/v1/blobdata/setBlobData", {
				"A client can store arbitrary json data by access key and tag",
				"params  : accesskey, tag , jsondata, textdata",
				"returns status"
			}
		});

		server->resource["/_identt/identity/api/v1/blobdata/setBlobData$"]["POST"]
		=[this,stptr](typename HttpServerT::RespPtr response, typename HttpServerT::ReqPtr request) {
			IDENTT_PARALLEL_ONE([this,stptr,response,request] {
				try {
					LOG(INFO) << request->path;
					std::string err;
					bool use_json = this->JsonRequest(request);
					if (!use_json) throw SydentException("Only Json Allowed",M_BAD_JSON);

					if (!stptr->is_ready.Get()) throw identt::BadDataException("System Not Ready");

					identt::query::SetBlobDataT bdata;
					this->HandleDataIn(request->content.string(), bdata.mutable_accessblob() );

					// action

					if (stptr->is_master.Get())
					{
						::identt::store::BlobDataService bservice;
						bservice.SetBlobDataAction(stptr, &bdata);
					} else {
						identt::hrpc::HrpcClient hclient;
						hclient.SendToMaster(stptr,::identt::hrpc::M_SETBLOBDATA,&bdata);
					}

					// aftermath
					if ( bdata.mutable_accessblob()->accessdenied() ) throw identt::BadDataException("Access Denied");
						
					std::string output = arr2json( {JsonElem("success",true)} );
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
	* HandleDataOut : convert the accessblob to json
	*
	* @param accessblob
	*   ::identt::query::AccessBlobT* blob data
	*
	* @param output
	*   std::string& output
	*
	* @return
	*   none
	*/
	void HandleDataOut(::identt::query::AccessBlobT* accessblob, std::string& output)
	{
		rapidjson::Document d;

		if (accessblob->accessdenied() ) {
			rapidjson::Pointer("/accessdenied").Set(d, true );
		} else {
			rapidjson::Pointer("/tag").Set(d, accessblob->tag() );
			rapidjson::Pointer("/textdata").Set(d, accessblob->textdata() );

			if (accessblob->jsondata().length()>1) {
				rapidjson::Pointer("/jsondata").Create(d);
				rapidjson::Document jd(&d.GetAllocator() ); // for jsondata
				jd.Parse(accessblob->jsondata().c_str());
				rapidjson::Pointer("/jsondata").Swap(d , jd );
			}
		}

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);
		output.clear();
		output.append(buffer.GetString(), buffer.GetSize());
	}

	/**
	* HandleDataIn : convert the json to accessblob
	*
	* @param input
	*   std::string input
	*
	* @param accessblob
	*   ::identt::query::AccessBlobT* blob data
	*
	* @return
	*   none
	*/
	void HandleDataIn(std::string input, ::identt::query::AccessBlobT* accessblob)
	{

		rapidjson::Document d;
		d.Parse<0>(input.c_str());
		if (d.HasParseError()) throw ::identt::query::SydentException("Bad Json Format",M_BAD_JSON);
		if (d.HasMember("accesskey")) if (d["accesskey"].IsString()) accessblob->set_accesskey( d["accesskey"].GetString() );
		if (d.HasMember("tag")) if (d["tag"].IsString()) accessblob->set_tag( d["tag"].GetString() );
		if (d.HasMember("textdata")) if (d["textdata"].IsString()) accessblob->set_textdata( d["textdata"].GetString() );
		// if jasondata exists make it a string
		if (d.HasMember("jsondata")) {
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			d["jsondata"].Accept(writer);
			std::string output;
			accessblob->set_jsondata( buffer.GetString(), buffer.GetSize());
		}

		// checks common
		std::vector<std::string> splitres;
		boost::algorithm::split(splitres, accessblob->accesskey() , boost::algorithm::is_any_of(":") );
		if (splitres.size()!=3) throw ::identt::BadDataException("Bad Accesskey");
		if (accessblob->tag().length()==0) throw ::identt::BadDataException("Blank Tag not allowed");
	}

};

} // namespace query
} // namespace identt

#endif // _IDENTT_QUERY_BLOBDATA_SERVICE_HPP_

