/**
 * @project identt
 * @file include/utils/HelpQuery.hpp
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
 *  HelpQuery.hpp :  Help Handler
 *
 */
#ifndef _IDENTT_UTILS_HELPQUERY_HPP_
#define _IDENTT_UTILS_HELPQUERY_HPP_

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace identt {
namespace utils {

class HelpQuery {
public:
	struct HelpT {
		const unsigned int scope;
		const std::string route;
		const std::vector<std::string> desc;
	};

	using pointer=std::shared_ptr<HelpQuery>;
	using LockT = boost::shared_mutex;
	using WriteLockT = boost::unique_lock< LockT >;
	using ReadLockT = boost::shared_lock< LockT >;
	using HelpListT = std::vector<HelpT>;

	/**
	* make noncopyable
	*/
	HelpQuery(const HelpQuery&) = delete;
	HelpQuery& operator=(const HelpQuery&) = delete;


	/**
	* constructor
	*
	*/
	HelpQuery() = default;

	/**
	* destructor
	*/
	virtual ~HelpQuery () {}

	/**
	* add : add one HelpT item
	*
	* @param h
	*   HelpT help item
	*
	* @return
	*   none
	*/
	void add(HelpT h)
	{
		WriteLockT lock (shared_lock);
		helplist.emplace_back(h);
	}

	/**
	* get : get a list of items matching scope
	*
	* @param scope
	*   HelpT help item
	*
	* @return
	*   HelpListT list of matching
	*/
	HelpListT get(const unsigned int scope)
	{
		HelpListT hl;
		ReadLockT lock (shared_lock);
		std::copy_if( helplist.begin() , helplist.end(), std::back_inserter(hl), [scope](const HelpT& h) {
			return (h.scope & scope);
		});
		return hl;
	}

protected:
	HelpListT helplist;
	LockT shared_lock;

};

} // namespace utils
} // namespace identt
#endif /* _IDENTT_UTILS_HELPQUERY_HPP_ */
