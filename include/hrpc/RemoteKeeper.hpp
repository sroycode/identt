/**
 * @project identt
 * @file include/hrpc/RemoteKeeper.hpp
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
 *  RemoteKeeper.hpp :  Help Handler
 *
 */
#ifndef _IDENTT_HRPC_REMOTE_KEEPER_HPP_
#define _IDENTT_HRPC_REMOTE_KEEPER_HPP_

#include <hrpc/HrpcClient.hpp>

namespace identt {
namespace hrpc {

class RemoteKeeper {
public:

	using pointer=std::shared_ptr<RemoteKeeper>;

	/**
	* constructor
	*
	*/
	RemoteKeeper(identt::utils::SharedTable::pointer s);

	/**
	* make noncopyable
	*/
	RemoteKeeper() = delete;
	RemoteKeeper(const RemoteKeeper&) = delete;
	RemoteKeeper& operator=(const RemoteKeeper&) = delete;

	/**
	* destructor
	*/
	virtual ~RemoteKeeper ();

	/**
	* SetHosts : get list of hosts
	*
	* @param state
	*   identt::hrpc::StateT* state
	*
	* @return
	*   none
	*/
	void SetHosts(identt::hrpc::StateT* state);

	/**
	* AddHost : add a single host
	*
	* @param rms
	*   identt::hrpc::RemoteT* rms
	*
	* @return
	*   none
	*/
	void AddHost(identt::hrpc::RemoteT* rms);

	/**
	* AddHost : add a single host
	*
	* @param name
	*   std::string name
	*
	* @param logcounter
	*   uint64_t logcounter
	*
	* @param updated_at
	*   uint64_t updated_at
	*
	* @return
	*   none
	*/
	void AddHost(std::string name, uint64_t logcounter, uint64_t updated_at);

	/**
	* GetHosts : get params and list of hosts
	*
	* @param state
	*   identt::hrpc::StateT* state
	*
	* @return
	*   none
	*/
	void GetHosts(identt::hrpc::StateT* state);

	/**
	* HostUpdate: update all hosts
	*
	* @return
	*   none
	*/
	void HostUpdate();

	/**
	* MasterUpdate: elect the master based on info received after cutoff
	*
	* @param state
	*   uint64_t cutoff 
	*
	* @return
	*   none
	*/
	void MasterUpdate(uint64_t cutoff);

private:
	identt::utils::SharedTable::pointer stptr;

};

} // namespace hrpc
} // namespace identt
#endif /* _IDENTT_HRPC_REMOTE_KEEPER_HPP_ */
