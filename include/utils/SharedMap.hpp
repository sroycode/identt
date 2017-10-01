/**
 * @project identt
 * @file include/utils/SharedMap.hpp
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
 *  SharedMap.hpp :  shared list
 *
 */

#ifndef _IDENTT_UTILS_SHARED_MAP_HPP_
#define _IDENTT_UTILS_SHARED_MAP_HPP_

#include <mutex>
#include <unordered_map>

namespace identt {
namespace utils {
template<typename Key, typename Value>
class SharedMap {
public:
	/**
	* emplace : emplace
	*
	* @param key
	*   const Key& key
	*
	* @param value
	*    const Value& value
	*
	* @return
	*   none
	*/
	void emplace(const Key& key, const Value& value)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		map_.emplace(key, value);
	}

	/**
	* replace : replace
	*
	* @param key
	*   const Key& key
	*
	* @param value
	*    const Value& value
	*
	* @param nullptr
	*    Value* out_value default nullptr
	*
	* @return
	*   bool
	*/
	bool replace(const Key& key, const Value& value, Value* out_value = nullptr)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		bool is_update = false;
		auto iter = map_.find(key);
		if (iter == map_.end()) {
			map_.emplace(key, value);
		} else {
			is_update = true;
			if (out_value != nullptr) {
				*out_value = iter->second;
			}
			iter->second = value;
		}

		return is_update;
	}

	/**
	* erase : erase
	*
	* @param key
	*   const Key& key
	*
	* @return
	*   none
	*/
	void erase(const Key& key)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		map_.erase(key);
	}

	/**
	* erase : erase
	*
	* @param key
	*   const Key& key
	*
	* @param value
	*    const Value& value
	*
	* @return
	*   bool
	*/
	bool erase(const Key& key, const Value& value)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto iter = map_.find(key);
		if (iter != map_.end()) {
			if (iter->second == value) {
				map_.erase(iter);
				return true;
			}
		}
		return false;
	}

	/**
	* for_each : for_each
	*
	* @param func
	*   const std::function<void(const Key&, const Value&)>& function
	*
	* @return
	*   none
	*/
	void for_each(const std::function<void(const Key&, const Value&)>& func)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& iter : map_) {
			func(iter.first, iter.second);
		}
	}

	/**
	* for_each_key : for_each key
	*
	* @param func
	*   const std::function<void(const Key&, const Value&)>& function
	*
	* @return
	*   none
	*/
	void for_each_key(const std::function<void(const Key&)>& func)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& iter : map_) {
			func(iter.first);
		}
	}

	/**
	* for_each_value : for_each value
	*
	* @param func
	*   const std::function<void(const Key&, const Value&)>& function
	*
	* @return
	*   none
	*/
	void for_each_value(const std::function<void(const Value&)>& func)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& iter : map_) {
			func(iter.second);
		}
	}

	/**
	* for_each_erase : for_each erase
	*
	* @param func
	*   const std::function<void(const Key&, const Value&)>& function
	*
	* @return
	*   none
	*/
	void for_each_erase(const std::function<bool(const Key&, const Value&)>& func)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto begin = map_.begin();
		while (begin != map_.end()) {
			if (func(begin->first, begin->second)) {
				begin = map_.erase(begin);
			} else {
				++begin;
			}
		}
	}

	/**
	* find : find
	*
	* @param key
	*   const Key& key
	*
	* @param out_value
	*    Value& out_value
	*
	* @return
	*   bool
	*/
	bool find(const Key& key, Value& out_value)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto iter = map_.find(key);
		if (iter != map_.end()) {
			out_value = iter->second;
			return true;
		}
		return false;
	}

	/**
	* exists : exists
	*
	* @param key
	*   const Key& key
	*
	* @return
	*   bool
	*/
	bool exists(const Key& key)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto iter = map_.find(key);
		if (iter != map_.end()) {
			return true;
		}
		return false;
	}

	/**
	*  clone : clone
	*
	* @return
	*   std::unordered_map<Key, Value>
	*/
	std::unordered_map<Key, Value> clone()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return map_;
	}

	/**
	*  clear : clear
	*
	* @return
	*   none
	*/
	void clear()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		map_.clear();
	}

	/**
	*  size : size
	*
	* @return
	*   std:;size_t
	*/
	std::size_t size()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return map_.size();
	}

private:
	std::unordered_map<Key, Value> map_;
	std::mutex mutex_;

};

} // namespace utils
} // namespace identt
#undef // _IDENTT_UTILS_SHARED_MAP_HPP_
