/**
 * @project identt
 * @file include/utils/SharedCounter.hpp
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
 *  SharedCounter.hpp :   Shared Counter
 *
 */
#ifndef _IDENTT_UTILS_SHARED_COUNTER_HPP_
#define _IDENTT_UTILS_SHARED_COUNTER_HPP_

#include <utils/SharedObject.hpp>

namespace identt {
namespace utils {
class SharedCounter : public SharedObject<uint64_t> {
public:
	using LockT = SharedObject<uint64_t>::LockT;
	using WriteLockT = SharedObject<uint64_t>::WriteLockT;
	using ReadLockT = SharedObject<uint64_t>::ReadLockT;

	/**
	* make noncopyable and remove default
	*/

	SharedCounter(const SharedCounter&) = delete;
	SharedCounter& operator=(const SharedCounter&) = delete;

	/**
	* Constructor : default
	*
	*/
	SharedCounter() : SharedObject<uint64_t>(0) {}

	/**
	* destructor
	*/
	virtual ~SharedCounter () {}

	/**
	* GetNext : increment and get next value
	*
	* @return
	*   uint64_t next value
	*/
	uint64_t GetNext()
	{
		WriteLockT writelock(mutex_);
		return ++t_;
	}

};
} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_SHARED_COUNTER_HPP_ */
