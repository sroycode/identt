/**
 * @project identt
 * @file src/store/ParAvionTable.cc
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
 *  ParAvionTable.cc :   Implementation for ParAvionTable  local messages to send
 *
 */
#include <store/ParAvionTable.hpp>
// template class identt::store::StoreTable<identt::store::ParAvionT>;

/**
* Constructor
*
*/
identt::store::ParAvionTable::ParAvionTable( identt::store::ParAvionTable::dbpointer trydb )
	: identt::store::ParAvionTable::ParAvionTable(
	trydb,
	K_PARAVION,
	"paravion",
	{  },
	{  }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string identt::store::ParAvionTable::GetKey(identt::store::ParAvionT* record, identt::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_PARAVION:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	}
	return key;
}
