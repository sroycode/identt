/**
 * @project identt
 * @file include/utils/SharedList.hpp
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
 *  SharedList.hpp :  shared list
 *
 */

#ifndef _IDENTT_UTILS_SHARED_LIST_HPP_
#define _IDENTT_UTILS_SHARED_LIST_HPP_

#include <list>
#include <mutex>

namespace identt {
namespace utils {
template<typename T>
class SharedList {
public:
	/**
	* emplace_back : emplace_back
	*
	* @param t
	*   const T& t
	*
	* @return
	*   none
	*/
	void emplace_back(const T& t)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		list_.emplace_back(t);
	}

	/**
	* front : front
	*
	* @return
	*   T
	*/
	T front()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return list_.front();
	}

	/**
	* pop_front : pop_front
	*
	* @return
	*   none
	*/
	void pop_front()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		list_.pop_front();
	}

	/**
	* clear : clear
	*
	* @return
	*   none
	*/
	void clear()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		list_.clear();
	}

	/**
	* empty : empty
	*
	* @return
	*   bool
	*/
	bool empty()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return list_.empty();
	}

	/**
	* size : size
	*
	* @return
	*   std::size_t
	*/
	std::size_t size()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return list_.size();
	}

private:
	std::list<T> list_;
	std::mutex mutex_;
};

} // namespace utils
} // namespace identt
#undef // _IDENTT_UTILS_SHARED_LIST_HPP_
