/**
 * @project identt
 * @file src/store/TransLogTable.cc
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
 *  TransLogTable.cc :   Implementation for TransLogTable  translog
 *
 */
#include <store/TransLogTable.hpp>
// template class identt::store::StoreTable<identt::store::TransLogT>;

/**
* Constructor
*
*/
identt::store::TransLogTable::TransLogTable( identt::store::TransLogTable::dbpointer trydb )
	: identt::store::TransLogTable::TransLogTable(
	trydb,
	K_LOGNODE,
	{  },
	{  }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string identt::store::TransLogTable::GetKey(identt::store::TransLogT* record, identt::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_LOGNODE:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	}
	return key;
}
