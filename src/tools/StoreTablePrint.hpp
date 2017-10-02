/**
 * @project identt
 * @file src/tools/StoreTablePrint.hpp
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
 *  StoreTablePrint.hpp :   Headers for printing StoreTable
 *
 */
#ifndef _IDENTT_TOOLS_STORE_TABLE_PRINT_HPP_
#define _IDENTT_TOOLS_STORE_TABLE_PRINT_HPP_

#include <iostream>
#include <store/StoreLevel.hpp>

namespace identt {
namespace store {
class StoreTablePrint : public StoreLevel {
public:
	using StoreLevel::dbpointer;
	using StoreLevel::StoreLevel;

	/**
	* make noncopyable and remove default
	*/

	StoreTablePrint() = delete;
	StoreTablePrint(const StoreTablePrint&) = delete;
	StoreTablePrint& operator=(const StoreTablePrint&) = delete;

	void PrintAll()
	{
		usemydb::Iterator* it = getDB()->NewIterator(usemydb::ReadOptions());
		for (it->SeekToFirst(); it->Valid(); it->Next()) {
			int keytype = std::stoi( std::string("0x") + it->key().ToString().substr(0,2),0,16);
			switch(keytype) {
			case K_EPHEMERALPUBLICKEY:	{
				EphemeralPublicKeyT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_EPHEMERALPUBLICKEY:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_GLOBALASSOC:	{
				GlobalAssocT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_GLOBALASSOC:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_INVITETOKEN:	{
				InviteTokenT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_INVITETOKEN:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_LOCALASSOC:	{
				LocalAssocT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_LOCALASSOC:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_PEER:	{
				PeerT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_PEER:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_PEERPUBKEY:	{
				PeerPubKeyT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_PEERPUBKEY:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_TOKENAUTH:	{
				TokenAuthT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_TOKENAUTH:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_VALIDATIONSESSION:	{
				ValidationSessionT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_VALIDATIONSESSION:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_PARAVION:	{
				ParAvionT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_PARAVION:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			case K_LOGNODE:	{
				TransactionT x;
				bool status = x.ParseFromString(it->value().ToString());
				if (status && x.id()>0) std::cout << "case K_LOGNODE:" << std::endl << x.DebugString() << std::endl;
				break;
			}
			}
		}
	}

};
} // namespace store
} // namespace identt
#endif /* _IDENTT_TOOLS_STORE_TABLE_PRINT_HPP_ */
