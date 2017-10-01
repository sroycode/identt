/**
 * @project identt
 * @file src/store/ValidationSessionTable.cc
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
 *  ValidationSessionTable.cc :   Implementation for ValidationSessionTable  validation session
 *
 */
#include <store/ValidationSessionTable.hpp>
// template class identt::store::StoreTable<identt::store::ValidationSessionT>;

/**
* Constructor
*
*/
identt::store::ValidationSessionTable::ValidationSessionTable( identt::store::ValidationSessionTable::dbpointer trydb )
	: identt::store::ValidationSessionTable::ValidationSessionTable(
	trydb,
	K_VALIDATIONSESSION,
	"validationsession",
	{ U_VALIDATIONSESSION_MEDIUM_ADDRESS_CLIENTSECRET },
	{  }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string identt::store::ValidationSessionTable::GetKey(identt::store::ValidationSessionT* record, identt::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_VALIDATIONSESSION:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	case U_VALIDATIONSESSION_MEDIUM_ADDRESS_CLIENTSECRET : {
		key = EncodeSecondaryKey<std::string,std::string,std::string>(keytype , record->medium(),record->address(),record->client_secret()  );
		break;
	}
	}
	return key;
}
