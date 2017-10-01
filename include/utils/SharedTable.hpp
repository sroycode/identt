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
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <utils/HelpQuery.hpp>
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

	using KeyRingT = std::unordered_map<std::string,std::shared_ptr<identt::crypto::CryptoBase> >;

	/**
	* public stable to be used directly , never updated after init by code
	*/
	HelpQuery httphelp;
	KeyRingT keyring;

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
	* setDB : set shared db pointer
	*
	* @param db
	*   dbpointer shared db pointer
	*/
	void setDB(dbpointer db_);

	/**
	* getDB : get shared db pointer
	*
	* @return
	*   dbpointer shared db pointer
	*/
	dbpointer getDB();

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

	/**
	* set_master : set atomic master
	*
	* @param io
	*   bool atomic master
	*/
	void set_master(bool r);

	/**
	* is_master : get atomic master
	*
	* @return
	*   bool atomic master
	*/
	bool is_master();

	/**
	* set_ready : set atomic ready
	*
	* @param r
	*   bool atomic ready
	*/
	void set_ready(bool r);

	/**
	* is_ready : get atomic ready
	*
	* @return
	*   bool atomic ready
	*/
	bool is_ready();

	/**
	* set_lastkey : set a primary key id
	*
	* @param key
	*   uint64_t key
	*
	* @return
	*   none
	*/
	void set_lastkey(uint64_t key);


	/**
	* get_lastkey: get last key id
	*
	* @return
	*   uint64_t key
	*/
	uint64_t get_lastkey();

	/**
	* GenKey: generate a primary key id
	*
	* @return
	*   uint64_t key
	*/
	uint64_t GenKey();

	/**
	* set_mailhost : set mailhost
	*
	* @param h
	*   std::string mailhost
	*/
	void set_mailhost(std::string h);

	/**
	* get_mailhost : get mailhost name
	*
	* @return
	*   std::string mailhost name
	*/
	std::string get_mailhost();

	/**
	* set_baseurl : set baseurl
	*
	* @param h
	*   std::string baseurl
	*/
	void set_baseurl(std::string h);

	/**
	* get_baseurl : get baseurl name
	*
	* @return
	*   std::string baseurl name
	*/
	std::string get_baseurl();

protected:

	bool ready;
	bool master;
	uint64_t number;
	std::string mailhost;
	std::string baseurl;
	dbpointer db;
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
