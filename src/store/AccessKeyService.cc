/**
 * @project identt
 * @file src/store/AccessKeyService.cc
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
 *  AccessKeyService.cc : Blob data implementation
 *
 */

#include <query/QueryBase.hpp>
#include <store/StoreBase.hpp>

#include <store/AccessKeyService.hpp>
#include <store/AccessKeyTable.hpp>

/**
* VerifyAccessKeyAction : verify the access key
*
*/
void identt::store::AccessKeyService::VerifyAccessKeyAction(::identt::utils::SharedTable::pointer stptr, std::string akey)
{
	std::vector<std::string> splitres;
	boost::algorithm::split(splitres, akey, boost::algorithm::is_any_of(":") );
	if (splitres.size()!=3) throw ::identt::BadDataException("Bad Accesskey");

	// accesskey
	::identt::store::AccessKeyT accesskey;
	::identt::store::AccessKeyTable accesskey_table{stptr->maindb.Get()};
	accesskey.set_medium( splitres[0] );
	accesskey.set_address ( splitres[1] );
	bool accesskey_found = accesskey_table.GetOne(&accesskey,::identt::store::U_ACCESSKEY_MEDIUM_ADDRESS);

	if (!accesskey_found) throw ::identt::BadDataException("Accesskey not found");
	if (accesskey.accesskey() != splitres[2] ) throw ::identt::BadDataException("Accesskey mismatch");
}
