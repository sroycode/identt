/**
 * @project identt
 * @file src/utils/SharedTable.cc
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
 *  SharedTable.cc :   Shared Objects implementation
 *
 */

#include <utils/SharedTable.hpp>

/**
* Constructor : default private
*
*/
identt::utils::SharedTable::SharedTable() {}


/**
* share : return instance
*
*/
identt::utils::SharedTable::pointer identt::utils::SharedTable::share()
{
	return shared_from_this();
}

/**
* destructor
*/
identt::utils::SharedTable::~SharedTable () {}

/**
* setIO : set shared io pointer
*
*/
void identt::utils::SharedTable::setIO(identt::utils::SharedTable::iopointer io_)
{
	WriteLockT writelock(class_mutex);
	io=io_;
}

/**
* getIO : get shared io pointer
*
*/
identt::utils::SharedTable::iopointer identt::utils::SharedTable::getIO()
{
	ReadLockT readlock(class_mutex);
	return io;
}
