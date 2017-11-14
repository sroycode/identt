/**
 * @project identt
 * @file src/http/SecureWebServer.hpp
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
 *  SecureWebServer.hpp :   Web Server Headers
 *
 */
#ifndef _IDENTT_HTTP_SECUREWEBSERVER_HPP_
#define _IDENTT_HTTP_SECUREWEBSERVER_HPP_

#include "utils/ServerBase.hpp"
#include "http/HttpServer.hpp"


namespace identt {
namespace http {

class SecureWebServer :
	public identt::utils::ServerBase,
	public std::enable_shared_from_this<SecureWebServer> {
public:
	using pointer=std::shared_ptr<SecureWebServer>;
	using HttpServerT=HttpServer<HTTPS>;

	/**
	* make noncopyable
	*/

	SecureWebServer() = delete;
	SecureWebServer(const SecureWebServer&) = delete;
	SecureWebServer& operator=(const SecureWebServer&) = delete;

	/**
	* create : static construction creates new first time
	*
	* @param io_service_
	*   boost::asio::io_service& io service pointer
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer sharedtable pointer
	*
	* @return
	*   none
	*/
	static pointer create(boost::asio::io_service& io_service_, identt::utils::SharedTable::pointer stptr)
	{
		pointer p(new SecureWebServer(io_service_,stptr));
		return p;
	}

	/**
	* share : return instance
	*
	* @return
	*   none
	*/
	pointer share()
	{
		return shared_from_this();
	}

	/**
	* destructor
	*/
	virtual ~SecureWebServer ();

	/**
	* init : initialize
	*
	* @param params
	*   ParamsListT parameters to init
	*
	* @return
	*   none
	*/
	void init(identt::utils::ServerBase::ParamsListT params);

	/**
	 * GetSection : section required
	 *
	 * @return
	 *   std::string section
	 */
	const static std::string GetSection();

	/**
	 * GetRequire : parameters required
	 *
	 * @return
	 *   ParamsListT
	 */
	const static identt::utils::ServerBase::ParamsListT GetRequire();

	/**
	* stop : shutdown
	*
	* @return
	*   none
	*/
	void stop();

protected:
	std::shared_ptr<HttpServerT> server;
	boost::asio::io_service& io_service;

private:
	bool is_init;

	/**
	* Constructor : private used Constructor
	*
	* @param io_service_
	*   boost::asio::io_service& io service pointer
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer sharedtable pointer
	*
	*/
	SecureWebServer(boost::asio::io_service& io_service, identt::utils::SharedTable::pointer stptr);

};
} // namespace http
} // namespace identt
#endif /* _IDENTT_HTTP_SECUREWEBSERVER_HPP_ */
