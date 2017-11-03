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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <utils/SharedObject.hpp>

namespace identt {
namespace hrpc {

class RemoteKeeper {
public:
	struct PropT {
		PropT(uint64_t l=0, uint64_t u=0) : logcounter(l), updated_at(u) {}
		uint64_t logcounter;
		uint64_t updated_at;
	};

	using pointer=std::shared_ptr<RemoteKeeper>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;
	using RemoteMapT = std::unordered_map<std::string, PropT>;
	using RemoteListT = std::vector<std::string>;
	using SharedBool = identt::utils::SharedObject<bool>;


	SharedBool is_ready;

	/**
	* constructor
	*
	*/
	RemoteKeeper() = default;


	/**
	* make noncopyable
	*/
	RemoteKeeper(const RemoteKeeper&) = delete;
	RemoteKeeper& operator=(const RemoteKeeper&) = delete;

	/**
	* destructor
	*/
	virtual ~RemoteKeeper () {}

	/**
	* Upsert : add new or update
	*
	* @param
	*   std::string host
	*
	* @param
	*   uint64_t logcounter
	*
	* @param
	*   uint64_t updated_at
	*
	* @return
	*   none
	*/
	void Upsert(std::string host, uint64_t logcounter, uint64_t updated_at)
	{
		WriteLockT lock (shared_lock);
		rmap[host]= {logcounter,updated_at};
	}

	/**
	* UpsertLogCounter : update log counter
	*
	* @param
	*   std::string host
	*
	* @param
	*   uint64_t logcounter
	*
	* @return
	*   none
	*/
	void UpsertLogCounter(std::string host, uint64_t logcounter)
	{
		WriteLockT lock (shared_lock);
		RemoteMapT::iterator it = rmap.find(host);
		if (it==rmap.end())
			rmap[host]= {logcounter,0};
		else
			it->second.logcounter=logcounter;
	}

	/**
	* UpsertUpdatedAt : update log counter
	*
	* @param
	*   std::string host
	*
	* @param
	*   uint64_t updated_at
	*
	* @return
	*   none
	*/
	void UpsertUpdatedAt(std::string host, uint64_t updated_at)
	{
		WriteLockT lock (shared_lock);
		RemoteMapT::iterator it = rmap.find(host);
		if (it==rmap.end())
			rmap[host]= {0,updated_at};
		else
			it->second.updated_at=updated_at;
	}

	/**
	* GetHosts : get list of hosts
	*
	* @return
	*   RemoteListT hosts list
	*/
	RemoteListT GetHosts()
	{
		ReadLockT lock (shared_lock);
		RemoteListT rlist;
		for (const auto& p : rmap) {
			rlist.emplace_back(p.first);
		}
		return rlist;
	}

private:
	RemoteMapT rmap;
	LockT shared_lock;

};

} // namespace hrpc
} // namespace identt
#endif /* _IDENTT_HRPC_REMOTE_KEEPER_HPP_ */
