/**
 * @project identt
 * @file include/utils/SharedObject.hpp
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
 *  SharedObject.hpp :   Shared Object
 *
 */
#ifndef _IDENTT_UTILS_SHARED_OBJECT_HPP_
#define _IDENTT_UTILS_SHARED_OBJECT_HPP_

#include <functional>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace identt {
namespace utils {
template <class T>
class SharedObject {
public:
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;

	/**
	* make noncopyable and remove default
	*/

	SharedObject(const SharedObject&) = delete;
	SharedObject& operator=(const SharedObject&) = delete;

	/**
	* Constructor : default
	*
	* @param t
	*   T initial value
	*
	*/
	SharedObject() {}
	SharedObject(T t) : t_(t) {}

	/**
	* destructor
	*/
	virtual ~SharedObject () {}

	/**
	* Set : set
	*
	* @param t
	*   T value to set
	*
	* @return
	*   none
	*/
	void Set(T t)
	{
		WriteLockT writelock(mutex_);
		t_=t;
	}

	/**
	* Get : get
	*
	* @return
	*   T value copy
	*/
	T Get()
	{
		ReadLockT readlock(mutex_);
		return t_;
	}

	T t_;
	LockT mutex_;

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHARED_OBJECT_HPP_ */
