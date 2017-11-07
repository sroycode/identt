/**
 * @project identt
 * @file include/utils/SharedPairMap.hpp
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
 *  SharedPairMap.hpp :   Shared String Map
 *
 */
#ifndef _IDENTT_UTILS_SHARED_PAIR_MAP_HPP_
#define _IDENTT_UTILS_SHARED_PAIR_MAP_HPP_

#include <unordered_map>
#include <utility>
#include <functional>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace identt {
namespace utils {
template<class KeyT,class LeftT,class RightT>
class SharedPairMap {
public:
	using ValueT = std::pair<LeftT,RightT>;
	using PairMapT = std::unordered_map<KeyT,ValueT>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;

	/**
	* make noncopyable and remove default
	*/

	SharedPairMap(const SharedPairMap&) = delete;
	SharedPairMap& operator=(const SharedPairMap&) = delete;

	/**
	* Constructor : default
	*
	*/
	SharedPairMap() {}

	/**
	* destructor
	*/
	virtual ~SharedPairMap () {}

	/**
	* AddOne : add one value
	*
	* @param key
	*   KeyT elem to add
	*
	* @param value
	*  ValueT elem to add
	*
	* @return
	*   none
	*/
	void AddOne(KeyT key, ValueT value)
	{
		WriteLockT writelock(mutex_);
		t_[key]=value;
	}

	/**
	* AddLeft : add one value
	*
	* @param key
	*   KeyT elem to add
	*
	* @param value
	*  ValueT elem to add
	*
	* @return
	*   none
	*/
	void AddLeft(KeyT key, LeftT value)
	{
		WriteLockT writelock(mutex_);
		typename PairMapT::iterator it = t_.find(key);
		if (it!=t_.end()) it.second.first = value;
		else t_[key]=std::make_pair(value,RightT());
	}

	/**
	* AddRight : add one value
	*
	* @param key
	*   KeyT elem to add
	*
	* @param value
	*  ValueT elem to add
	*
	* @return
	*   none
	*/
	void AddRight(KeyT key, RightT value)
	{
		WriteLockT writelock(mutex_);
		typename PairMapT::iterator it = t_.find(key);
		if (it!=t_.end()) it.second.second = value;
		else t_[key]=std::make_pair(LeftT(),value);
	}

	/**
	* Swap : swap from map
	*
	* @param m
	*   PairMap map to swap
	*
	* @return
	*   none
	*/
	void Swap(PairMapT m)
	{
		WriteLockT writelock(mutex_);
		t_.swap(m);
	}

	/**
	* AddMany : add from map by address
	*
	* @param m
	*   const PairMap& map to get from
	*
	* @return
	*   none
	*/
	void AddMany(const PairMapT& m)
	{
		WriteLockT writelock(mutex_);
		for (auto& p : m) {
			t_[p.first]=p.second;
		}
	}

	/**
	* AddMany : add from map by lvalue ref
	*
	* @param m
	*   const PairMap&& map to get from
	*
	* @return
	*   none
	*/
	void AddMany(PairMapT&& m)
	{
		WriteLockT writelock(mutex_);
		for (auto& p : m) {
			t_[p.first]=p.second;
		}
	}

	/**
	* GetValues : get values
	*
	* @param key
	*   KeyT&& elem to get
	*
	* @param left
	*   LeftT& elem to get
	*
	* @param right
	*   RightT& elem to get
	*
	* @return
	*   bool true if exists
	*/
	bool GetValues(KeyT&& key, LeftT& left, RightT& right)
	{
		ReadLockT readlock(mutex_);
		typename PairMapT::const_iterator it = t_.find(key);
		bool stat = (it!=t_.end());
		left= stat ? it.second.first : LeftT();
		right= stat ? it.second.second : RightT();
		return stat;
	}

	/**
	* GetValues : get values by address
	*
	* @param key
	*   KeyT& elem to get by address
	*
	* @param left
	*   LeftT& elem to get
	*
	* @param right
	*   RightT& elem to get
	*
	* @return
	*   bool true if exists
	*/
	bool GetValues(const KeyT& key, LeftT& left, RightT& right)
	{
		ReadLockT readlock(mutex_);
		typename PairMapT::const_iterator it = t_.find(key);
		bool stat = (it!=t_.end());
		left= stat ? it.second.first : LeftT();
		right= stat ? it.second.second : RightT();
		return stat;
	}

	/**
	* GetCopy : get a copy of the map
	*
	* @return
	*   PairMapT map
	*/
	PairMapT GetCopy()
	{
		ReadLockT readlock(mutex_);
		return t_;
	}

	/**
	* GetSize: get size of set
	*
	* @return
	*   size_t size
	*/
	size_t GetSize()
	{
		ReadLockT readlock(mutex_);
		return t_.size();
	}

	/**
	* Reset: clear the set
	*
	* @return
	*   none
	*/
	void Reset()
	{
		WriteLockT writelock(mutex_);
		t_.clear();
	}


protected:
	PairMapT t_;
	LockT mutex_;

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHARED_PAIR_MAP_HPP_ */
