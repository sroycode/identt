/**
 * @project identt
 * @file src/hrpc/SyncServer.hpp
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
 *  SyncServer.hpp :   Work Storage Server Headers
 *
 */
#ifndef _IDENTT_HRPC_SYNC_SERVER_HPP_
#define _IDENTT_HRPC_SYNC_SERVER_HPP_

#include <store/StoreBase.hpp>
#include <utils/ServerBase.hpp>

namespace identt {
namespace hrpc {

class SyncServer :
	virtual public identt::utils::ServerBase,
	virtual public std::enable_shared_from_this<SyncServer> {

public:
	using pointer=std::shared_ptr<SyncServer>;

	/**
	* make noncopyable
	*/

	SyncServer() = delete;
	SyncServer(const SyncServer&) = delete;
	SyncServer& operator=(const SyncServer&) = delete;

	/**
	* create : static construction creates new first time
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer stptr
	*
	* @return
	*   none
	*/
	static pointer create(identt::utils::SharedTable::pointer stptr)
	{
		pointer p(new SyncServer(stptr));
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
	virtual ~SyncServer ();

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
	* init : initialize rocksdb and others
	*
	* @param params
	*   ParamsListT parameters to init backend
	*
	* @return
	*   none
	*/
	void init(identt::utils::ServerBase::ParamsListT params);

	/**
	* stop : shutdown
	*
	* @return
	*   none
	*/
	void stop();

private:

	/**
	* Constructor : private default Constructor
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer stptr
	*
	*/
	SyncServer(identt::utils::SharedTable::pointer stptr);

	/**
	* SyncFromMaster : sync data from master
	*
	* @param stptr
	*   identt::utils::SharedTable::pointer stptr
	*
	* @param chunksize
	*   size_t chunk size of request
	*
	* @return
	*   none
	*/
	void SyncFromMaster(identt::utils::SharedTable::pointer stptr, size_t chunksize);

};
} // namespace hrpc
} // namespace identt
#endif /* _IDENTT_HRPC_SYNC_SERVER_HPP_ */

