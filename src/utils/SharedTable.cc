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
identt::utils::SharedTable::SharedTable() : ready(false), master(false), number(1) {}


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
* setDB : set shared db pointer
*
*/
void identt::utils::SharedTable::setDB(identt::utils::SharedTable::dbpointer db_)
{
	WriteLockT writelock(class_mutex);
	db=db_;
}

/**
* getDB : get shared db pointer
*
*/
identt::utils::SharedTable::dbpointer identt::utils::SharedTable::getDB()
{
	ReadLockT readlock(class_mutex);
	return db;
}

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

/**
* set_master : set atomic master
*
*/
void identt::utils::SharedTable::set_master(bool r)
{
	WriteLockT writelock(class_mutex);
	master=r;
}

/**
* is_master : get atomic master
*
*/
bool identt::utils::SharedTable::is_master()
{
	ReadLockT readlock(class_mutex);
	return master;
}

/**
* set_ready : set atomic ready
*
*/
void identt::utils::SharedTable::set_ready(bool r)
{
	WriteLockT writelock(class_mutex);
	ready=r;
}

/**
* is_ready : get atomic ready
*
*/
bool identt::utils::SharedTable::is_ready()
{
	ReadLockT readlock(class_mutex);
	return ready;
}

/**
* set_lastkey : set a primary key id
*
*/
void identt::utils::SharedTable::set_lastkey(uint64_t key)
{
	WriteLockT writelock(number_mutex);
	number=key;
}


/**
* get_lastkey: get last key id
*
*/
uint64_t identt::utils::SharedTable::get_lastkey()
{
	ReadLockT readlock(number_mutex);
	return number;
}


/**
* GenKey: generate a primary key id
*
*/
uint64_t identt::utils::SharedTable::GenKey()
{
	WriteLockT writelock(number_mutex);
	++number;
	return number;
}

/**
* set_mailhost : set mailhost
*
*/
void identt::utils::SharedTable::set_mailhost(std::string h)
{
	WriteLockT writelock(class_mutex);
	mailhost=h;
}

/**
* get_mailhost : get mailhost name
*
*/
std::string identt::utils::SharedTable::get_mailhost()
{
	ReadLockT readlock(class_mutex);
	return mailhost;
}

/**
* set_baseurl : set baseurl
*
*/
void identt::utils::SharedTable::set_baseurl(std::string h)
{
	WriteLockT writelock(class_mutex);
	baseurl=h;
}

/**
* get_baseurl : get baseurl name
*
*/
std::string identt::utils::SharedTable::get_baseurl()
{
	ReadLockT readlock(class_mutex);
	return baseurl;
}


