/**
 * @project identt
 * @file src/store/EphemeralPublicKeyTable.cc
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
 *  EphemeralPublicKeyTable.cc :   Implementation for EphemeralPublicKeyTable  ephemeral public key
 *
 */
#include <store/EphemeralPublicKeyTable.hpp>
// template class identt::store::StoreTable<identt::store::EphemeralPublicKeyT>;

/**
* Constructor
*
*/
identt::store::EphemeralPublicKeyTable::EphemeralPublicKeyTable( identt::store::EphemeralPublicKeyTable::dbpointer trydb )
	: identt::store::EphemeralPublicKeyTable::EphemeralPublicKeyTable(
	trydb,
	K_EPHEMERALPUBLICKEY,
	{ U_EPHEMERALPUBLICKEY_PUBLICKEY },
	{  }
	)

{}
/**
* GetKey: get a secondary key
*
*/
std::string identt::store::EphemeralPublicKeyTable::GetKey(identt::store::EphemeralPublicKeyT* record, identt::store::KeyTypeE keytype, bool pre)
{

	std::string key;
	switch (keytype) {
	default:
	case K_EPHEMERALPUBLICKEY:
		key = EncodePrimaryKey(PrimaryKey,record->id());
		break;
	case U_EPHEMERALPUBLICKEY_PUBLICKEY : {
		key = EncodeSecondaryKey<std::string>(keytype , record->public_key()  );
		break;
	}
	}
	return key;
}
