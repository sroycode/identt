/**
 * @project identt
 * @file include/utils/SharedStringMap.hpp
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
 *  SharedStringMap.hpp :   Shared Counter
 *
 */
#ifndef _IDENTT_UTILS_SHARED_STRING_MAP_HPP_
#define _IDENTT_UTILS_SHARED_STRING_MAP_HPP_

#include <utils/SharedObject.hpp>
#include <string>
#include <unordered_map>

namespace identt {
namespace utils {
template<class T>
class SharedStringMap : public SharedObject<std::unordered_map<std::string,T> > {
public:
	using StringMapT = std::unordered_map<std::string,T>;
	using LockT = SharedObject<StringMapT>::LockT;
	using WriteLockT = SharedObject<StringMapT>::WriteLockT;
	using ReadLockT = SharedObject<StringMapT>::ReadLockT;

	/**
	* make noncopyable and remove default
	*/

	SharedStringMap(const SharedStringMap&) = delete;
	SharedStringMap& operator=(const SharedStringMap&) = delete;

	/**
	* Constructor : default
	*
	*/
	SharedStringMap() {}

	/**
	* destructor
	*/
	virtual ~SharedStringMap () {}

	/**
	* Add : add one value
	*
	* @param key
	*   std::string elem to add
	*
	* @param value
	*  T elem to add
	*
	* @return
	*   none
	*/
	void Add(std::string key, T value)
	{
		WriteLockT writelock(mutex_);
		t_[key]=value;
	}

	/**
	* Add : add one value by address
	*
	* @param key
	*   std::string& elem to add
	*
	* @param value
	*  T elem to add
	*
	* @return
	*   none
	*/
	void Add(std::string& key, T value)
	{
		WriteLockT writelock(mutex_);
		t_[key]=value;
	}


	/**
	* Get : add one value 
	*
	* @param key
	*   std::string elem to get
	*
	* @return
	*   none
	*/
	T Get(std::string key)
	{
		ReadLockT readlock(mutex_);
		StringMapT::const_iterator it = t_.find(key);
		return (it!=t_.end()) ? it->second : T() ;
	}

	/**
	* Get : add one value by address
	*
	* @param key
	*   std::string& elem to get
	*
	* @return
	*   none
	*/
	T Get(std::string& key)
	{
		ReadLockT readlock(mutex_);
		StringMapT::const_iterator it = t_.find(key);
		return (it!=t_.end()) ? it->second : T() ;
	}

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHARED_STRING_MAP_HPP_ */
