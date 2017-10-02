/**
 * @project identt
 * @file src/store/GlobalAssocTable.cc
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
 *  GlobalAssocTable.cc :   Implementation for GlobalAssocTable global associations
 *
 */
#include <store/GlobalAssocTable.hpp>
// template class identt::store::StoreTable<identt::store::GlobalAssocT>;

/**
* Constructor
*
*/
identt::store::GlobalAssocTable::GlobalAssocTable( identt::store::GlobalAssocTable::dbpointer trydb )
	: identt::store::GlobalAssocTable::GlobalAssocTable(
	trydb,
	K_GLOBALASSOC,
	{ U_GLOBALASSOC_ORIGINSERVER_ORIGINID },
	{ I_GLOBALASSOC_MEDIUM_ADDRESS }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string identt::store::GlobalAssocTable::GetKey(identt::store::GlobalAssocT* record, identt::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_GLOBALASSOC:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	case U_GLOBALASSOC_ORIGINSERVER_ORIGINID : {
		key = EncodeSecondaryKey<std::string,uint64_t>(keytype , record->origin_server(),record->origin_id()  );
		break;
	}
	case I_GLOBALASSOC_MEDIUM_ADDRESS : {
		key = (pre) ?
		      EncodeSecondaryKey<std::string,std::string>(keytype , record->medium(),record->address() )
			    : EncodeSecondaryKey<std::string,std::string,uint64_t>(keytype , record->medium(),record->address(),record->id() );
		break;
	}
	}
	return key;
}
