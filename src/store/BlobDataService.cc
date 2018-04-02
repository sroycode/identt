/**
 * @project identt
 * @file src/store/BlobDataService.cc
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
 *  BlobDataService.cc : Blob data implementation
 *
 */

#include <query/QueryBase.hpp>
#include <store/StoreBase.hpp>

#include <store/BlobDataService.hpp>

#include <store/ValidationSessionTable.hpp>
#include <store/AccessKeyTable.hpp>
#include <store/BlobStoreTable.hpp>
#include <store/StoreTrans.hpp>

#include <utils/sole.hpp>

/**
* GetAccessKeyAction : get the access key
*
*/
void identt::store::BlobDataService::GetAccessKeyAction(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::GetAccessDataT* gva)
{
	::identt::query::SubmitTokenT* subtok = gva->mutable_subtok();
	// check
	uint64_t sid = std::stoul( subtok->sid() );
	uint64_t currtime = IDENTT_CURRTIME_MS;
	if (sid==0)
		throw ::identt::query::SydentException("sid value required", M_MISSING_PARAMS);
	if (subtok->client_secret().length()==0)
		throw ::identt::query::SydentException("client_secret value required", M_MISSING_PARAMS);
	// start
	::identt::store::ValidationSessionT valsession;
	valsession.set_id ( sid );
	::identt::store::ValidationSessionTable valsession_table{stptr->maindb.Get()};
	bool validation_found = valsession_table.GetOne(&valsession,::identt::store::K_VALIDATIONSESSION);
	if (!validation_found || valsession.client_secret() != subtok->client_secret())
		throw ::identt::query::SydentException("No valid session was found matching that sid and client secret",M_NO_VALID_SESSION);
	if (valsession.validated() <=0) {
		// this will throw anyway
		if (THREEPID_SESSION_VALID_LIFETIME_MS + valsession.mtime() > currtime )
			throw ::identt::query::SydentException("This validation session has expired: call requestToken again",M_SESSION_EXPIRED);
		throw ::identt::query::SydentException("This validation session has not yet been completed",M_SESSION_NOT_VALIDATED);
	}

	// accesskey
	::identt::store::AccessKeyT accesskey;
	::identt::store::AccessKeyTable accesskey_table{stptr->maindb.Get()};
	accesskey.set_medium( valsession.medium() );
	accesskey.set_address ( valsession.address() );
	bool accesskey_found = accesskey_table.GetOne(&accesskey,::identt::store::U_ACCESSKEY_MEDIUM_ADDRESS);

	// add accesskey if not found create

	if (!accesskey_found) {
		::identt::store::TransactionT trans;
		accesskey.set_id( stptr->maincounter.GetNext() );
		accesskey.set_medium( valsession.medium() );
		accesskey.set_address ( valsession.address() );
		std::string tok = boost::algorithm::erase_all_copy(sole::uuid4().str(), ":");
		accesskey.set_accesskey( tok );
		bool status = accesskey_table.AddRecord(&accesskey, &trans,accesskey_found);
		if (!status) throw ::identt::BadDataException("Cannot Insert accesskey");
		// transaction , throws on fail
		::identt::store::StoreTrans storetrans;
		storetrans.Commit(stptr,&trans,true); // as master
	}

	// for outgoing
	::identt::query::AccessBlobT* accessblob = gva->mutable_accessblob();
	std::string outkey = accesskey.medium() + ":" + accesskey.address() + ":" + accesskey.accesskey();
	accessblob->set_accesskey( outkey );
}

/**
* SetBlobDataAction : set the access key
*
*/
void identt::store::BlobDataService::SetBlobDataAction(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::SetBlobDataT* bdata)
{
	::identt::query::AccessBlobT* accessblob = bdata->mutable_accessblob();
	std::vector<std::string> splitres;
	boost::algorithm::split(splitres, accessblob->accesskey() , boost::algorithm::is_any_of(":") );
	if (splitres.size()!=3) throw ::identt::BadDataException("Bad Accesskey");

	// accesskey
	::identt::store::AccessKeyT accesskey;
	::identt::store::AccessKeyTable accesskey_table{stptr->maindb.Get()};
	accesskey.set_medium( splitres[0] );
	accesskey.set_address ( splitres[1] );
	bool accesskey_found = accesskey_table.GetOne(&accesskey,::identt::store::U_ACCESSKEY_MEDIUM_ADDRESS);

	if (!accesskey_found) throw ::identt::BadDataException("Accesskey not found");
	if (accesskey.accesskey() != splitres[2] ) throw ::identt::BadDataException("Accesskey mismatch");

	// update blobstore

	::identt::store::TransactionT trans;
	::identt::store::BlobStoreT blobstore;
	::identt::store::BlobStoreTable blobstore_table{stptr->maindb.Get()};

	blobstore.set_keyid( accesskey.id() );
	blobstore.set_tag ( accessblob->tag() );
	bool blobstore_found = blobstore_table.GetOne(&blobstore,::identt::store::U_BLOBSTORE_KEYID_TAG);
	if (!blobstore_found) {
		blobstore.set_id( stptr->maincounter.GetNext() );
		blobstore.set_keyid( accesskey.id() );
		blobstore.set_tag ( accessblob->tag() );
	}

	blobstore.set_jsondata ( accessblob->jsondata() );
	blobstore.set_textdata ( accessblob->textdata() );

	bool status = blobstore_table.AddRecord(&blobstore, &trans,blobstore_found);
	if (!status) throw ::identt::BadDataException("Cannot Insert data");

	::identt::store::StoreTrans storetrans;
	storetrans.Commit(stptr,&trans,true); // as master
}


/**
* GetBlobDataAction : get the access key
*
*/
void identt::store::BlobDataService::GetBlobDataAction(
    ::identt::utils::SharedTable::pointer stptr, ::identt::query::GetBlobDataT* bdata)
{
	::identt::query::AccessBlobT* accessblob = bdata->mutable_accessblob();
	std::vector<std::string> splitres;
	boost::algorithm::split(splitres, accessblob->accesskey() , boost::algorithm::is_any_of(":") );
	if (splitres.size()!=3) throw ::identt::BadDataException("Bad Accesskey");

	// accesskey
	::identt::store::AccessKeyT accesskey;
	::identt::store::AccessKeyTable accesskey_table{stptr->maindb.Get()};
	accesskey.set_medium( splitres[0] );
	accesskey.set_address ( splitres[1] );
	bool accesskey_found = accesskey_table.GetOne(&accesskey,::identt::store::U_ACCESSKEY_MEDIUM_ADDRESS);

	if (!accesskey_found) throw ::identt::BadDataException("Accesskey not found");
	if (accesskey.accesskey() != splitres[2] ) throw ::identt::BadDataException("Accesskey mismatch");

	// update blobstore

	::identt::store::BlobStoreT blobstore;
	::identt::store::BlobStoreTable blobstore_table{stptr->maindb.Get()};

	blobstore.set_keyid( accesskey.id() );
	blobstore.set_tag ( accessblob->tag() );
	bool blobstore_found = blobstore_table.GetOne(&blobstore,::identt::store::U_BLOBSTORE_KEYID_TAG);
	if (!blobstore_found) throw ::identt::BadDataException("Tag not found");

	accessblob->set_jsondata ( blobstore.jsondata() );
	accessblob->set_textdata ( blobstore.textdata() );

}
