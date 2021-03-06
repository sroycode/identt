/**
 * @project identt
 * @file include/utils/SharedStringSet.hpp
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
 *  SharedStringSet.hpp :   Shared Counter
 *
 */
#ifndef _IDENTT_UTILS_SHARED_STRING_SET_HPP_
#define _IDENTT_UTILS_SHARED_STRING_SET_HPP_

#include <utils/SharedObject.hpp>
#include <string>
#include <set>

namespace identt {
namespace utils {
class SharedStringSet : public SharedObject<std::set<std::string> > {
public:
	using StringSet = std::set<std::string>;
	using LockT = SharedObject<StringSet>::LockT;
	using WriteLockT = SharedObject<StringSet>::WriteLockT;
	using ReadLockT = SharedObject<StringSet>::ReadLockT;

	/**
	* make noncopyable and remove default
	*/

	SharedStringSet(const SharedStringSet&) = delete;
	SharedStringSet& operator=(const SharedStringSet&) = delete;

	/**
	* Constructor : default
	*
	*/
	SharedStringSet() {}

	/**
	* destructor
	*/
	virtual ~SharedStringSet () {}

	/**
	* Add : add one value
	*
	* @param elem
	*   std::string elem to add
	*
	* @return
	*   none
	*/
	void Add(std::string elem)
	{
		WriteLockT lock(mutex_);
		t_.insert(elem);
	}

	/**
	* GetSize: get size of set
	*
	* @return
	*   size_t size
	*/
	size_t GetSize()
	{
		ReadLockT lock(mutex_);
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
		WriteLockT lock(mutex_);
		t_.clear();
	}

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHARED_STRING_SET_HPP_ */
