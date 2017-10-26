/**
 * @project identt
 * @file src/store/MailSmsService.cc
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
 *  MailSmsService.cc : Implementation of Mail SmsService
 *
 */

#include <store/MailSmsService.hpp>

#include <query/bin2ascii.h>
#include <store/ParAvionTable.hpp>
#include <store/MailSentTable.hpp>

#include <store/StoreTrans.hpp>

/**
* PendingAction : Get Pending as required
*
*/
void identt::store::MailSmsService::PendingAction(::identt::utils::SharedTable::pointer stptr, identt::mail::MailQueryT* mailq)
{

	::identt::store::ParAvionT paravion;
	::identt::store::ParAvionTable paravion_table{stptr->maindb.Get()};

	// todo: copy to mailsent table
	// ::identt::store::MailSentT mailsent;
	// ::identt::store::MailSentTable mailsent_table{stptr->maindb.Get()};

	// delete done records
	if (mailq->payload_size()>0) {
		::identt::store::TransactionT trans;
		for (auto i=0; i<mailq->payload_size(); ++i) {
			paravion.set_id(mailq->mutable_payload(i)->id());
			paravion_table.DelRecord(&paravion,&trans);
		}

		// transaction , throws on fail
		::identt::store::StoreTrans storetrans;
		storetrans.Commit(stptr,&trans);

		mailq->clear_payload();
	}

	// fetch new
	::identt::store::ParAvionTable::MapT records;
	uint64_t lastid = (mailq->lastid()>0) ? mailq->lastid()+1 :0;
	uint64_t limit = (mailq->limit()>100) ? 100 : mailq->limit();
	bool hasrecords = paravion_table.ScanTable(&records,lastid,limit);
	lastid=0;
	for (auto it= records.begin() ; it!=records.end(); ++it) {
		if (it->first > lastid) lastid= it->first;
		it->second.mutable_payload()->Swap(mailq->add_payload());
	}
	mailq->set_lastid(lastid);
}
