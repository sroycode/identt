/**
 * @project identt
 * @file include/utils/SharedTable.hpp
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
 *  SharedTable.hpp :   Shared pointer to sharedtable object
 *
 */
#ifndef _IDENTT_UTILS_SHAREDTABLE_HPP_
#define _IDENTT_UTILS_SHAREDTABLE_HPP_

#include <memory>
#include <vector>
#include <chrono>
#include <random>

#include <utils/SharedCounter.hpp>
#include <utils/SharedPairMap.hpp>
#include <utils/SharedMap.hpp>

#include <store/StoreLevel.hpp>
#include <store/StoreCache.hpp>
#include <crypto/CryptoBase.hpp>

#define IDENTT_CURRTIME_MS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

namespace identt {
namespace utils {
class SharedTable : public std::enable_shared_from_this<SharedTable> {
public:
	using pointer=std::shared_ptr<SharedTable>;
	using dbpointer=identt::store::StoreLevel::dbpointer;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;
	using SharedString = SharedObject<std::string>;
	using SharedDBPointer = SharedObject<dbpointer>;
	using SharedBool = SharedObject<bool>;
	using SharedInt = SharedObject<size_t>;

	using SharedRemote = SharedPairMap<std::string,uint64_t,uint64_t>;
	using RemoteMapT = SharedPairMap<std::string,uint64_t,uint64_t>::PairMapT;

	using SharedTrans = SharedMap<uint64_t,std::string>;
	using KeyRingT = std::unordered_map<std::string,std::shared_ptr<identt::crypto::CryptoBase> >;

	using SharedCache = identt::store::StoreCache::pointer;

	/**
	* public stable to be used directly , never updated after init by code
	*/
	KeyRingT keyring;

	// map of shared followers
	SharedRemote remotes;

	// map of shared transactions
	SharedTrans transactions;

	// shared cache
	SharedCache dbcache;

	// string shared
	SharedString master;
	SharedString shared_secret;
	SharedString hostname;
	SharedString baseurl;
	SharedString thisurl;
	SharedString lastslave;
	// counter shared
	SharedCounter maincounter;
	SharedCounter logcounter;
	// database pointers
	SharedDBPointer maindb;
	SharedDBPointer logdb;
	// booleans
	SharedBool is_master;
	SharedBool is_ready;
	SharedBool lookup_requires_key;
	SharedBool invite_requires_key;
	SharedBool dont_send_sms;
	SharedBool lookup_uses_local;

	/**
	* make noncopyable
	*/

	SharedTable(const SharedTable&) = delete;
	SharedTable& operator=(const SharedTable&) = delete;

	/**
	* create : static construction creates new first time
	*
	* @return
	*   pointer
	*/
	static pointer create()
	{
		pointer p(new SharedTable());
		return p;
	}

	/**
	* share : return instance
	*
	* @return
	*   pointer
	*/
	pointer share()
	{
		return shared_from_this();
	}

	/**
	* destructor
	*/
	virtual ~SharedTable () {}

private:

	LockT class_mutex;
	LockT number_mutex;

	/**
	* Constructor : default private
	*
	* @return
	*   none
	*/
	SharedTable()
		: dbcache ( identt::store::StoreCache::create() )
	{}

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHAREDTABLE_HPP_ */
