/**
 * @project identt
 * @file include/utils/ServerBase.hpp
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
 *  ServerBase.hpp :   Server ABC
 *
 */
#ifndef _IDENTT_UTILS_SERVERBASE_HPP_
#define _IDENTT_UTILS_SERVERBASE_HPP_

#include "BaseUtils.hpp"

#include <csignal>
#include <vector>
#include <memory>
#include <initializer_list>
#include "SharedTable.hpp"

namespace identt {
namespace utils {
class ServerBase {
public:
	using ParamsListT=std::vector<std::string>;

	/**
	* make noncopyable
	*/

	ServerBase(const ServerBase&) = delete;
	ServerBase& operator=(const ServerBase&) = delete;

	/**
	* destructor
	*/
	virtual ~ServerBase () {}

	/**
	* init : initialize
	*
	* @param params
	*   ParamsListT parameters to init
	*
	* @return
	*   none
	*/
	virtual void init(ParamsListT params)=0;

	/**
	 * GetSection : section required
	 *
	 * @return
	 *   std::string section
	 */
	const static std::string GetSection();

	/**
	 * GetRequire : parameters required
	 *
	 * @return
	 *   ParamsListT
	 */
	const static ParamsListT GetRequire();

	/**
	* stop : shutdown
	*
	* @return
	*   none
	*/
	virtual void stop()=0;

protected:
	bool is_init;
	SharedTable::pointer sharedtable;

	/**
	* Constructor : private
	*
	* @param sharedtable_
	*   SharedTable::pointer sharedtable pointer
	*
	* @return
	*   none
	*/
	ServerBase(SharedTable::pointer sharedtable_)
		: is_init(false),
		  sharedtable(sharedtable_->share())
	{}

};
} // namespace utils
} // namespace identt

#endif /* _IDENTT_UTILS_SERVERBASE_HPP_ */
