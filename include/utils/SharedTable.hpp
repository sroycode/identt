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
 *  SharedTable.hpp :   Shared pointer to sharedtable object called pricetable
 *
 */
#ifndef _IDENTT_UTILS_SHAREDTABLE_HPP_
#define _IDENTT_UTILS_SHAREDTABLE_HPP_

#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <random>
#include <boost/asio.hpp>

#include <utils/SharedObject.hpp>
#include <utils/SharedCounter.hpp>
#include <store/StoreLevel.hpp>
#include <crypto/CryptoBase.hpp>


namespace identt {
namespace utils {
class SharedTable : public std::enable_shared_from_this<SharedTable> {
public:
	using pointer=std::shared_ptr<SharedTable>;
	using dbpointer=identt::store::StoreLevel::dbpointer;
	using iopointer = std::shared_ptr<boost::asio::io_service>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;
	using SharedString = SharedObject<std::string>;
	using SharedDBPointer = SharedObject<dbpointer>;
	using SharedBool = SharedObject<bool>;

	using KeyRingT = std::unordered_map<std::string,std::shared_ptr<identt::crypto::CryptoBase> >;

	/**
	* public stable to be used directly , never updated after init by code
	*/
	KeyRingT keyring;
	// string shared
	SharedString master;
	SharedString shared_secret;
	SharedString hostname;
	SharedString baseurl;
	SharedString thisurl;
	// counter shared
	SharedCounter maincounter;
	SharedCounter logcounter;
	// database pointers
	SharedDBPointer maindb;
	SharedDBPointer logdb;
	// booleans
	SharedBool is_master;
	SharedBool is_ready;

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
	pointer share();

	/**
	* destructor
	*/
	virtual ~SharedTable ();

	/**
	* setIO : set shared io pointer
	*
	* @param io
	*   iopointer shared io pointer
	*/
	void setIO(iopointer io_);

	/**
	* getIO : get shared io pointer
	*
	* @return
	*   iopointer shared io pointer
	*/
	iopointer getIO();

private:

	iopointer io;
	LockT class_mutex;
	LockT number_mutex;

	/**
	* Constructor : default private
	*
	* @return
	*   none
	*/
	SharedTable();

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHAREDTABLE_HPP_ */
