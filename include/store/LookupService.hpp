/**
 * @project identt
 * @file include/query/LookupService.hpp
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
 *  LookupService.hpp : Header for email msisdn service
 *
 */
#ifndef _IDENTT_STORE_LOOKUP_SERVICE_HPP_
#define _IDENTT_STORE_LOOKUP_SERVICE_HPP_

#include <query/SignedJson.hpp>
#include <store/StoreBase.hpp>
#include <mutex>

namespace identt {
namespace store {

class LookupService : public StoreBase, public ::identt::query::SignedJson {
public:

	/**
	* LookupAction : Service Endpoint Lookup
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param query
	*   ::identt::query::LookupDataT*  lact
	*
	* @return
	*   none
	*/
	void LookupAction(
	    ::identt::utils::SharedTable::pointer stptr,
	    ::identt::query::LookupDataT* lact);

	/**
	* BulkLookupAction : Service Endpoint Lookup
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param query
	*   ::identt::query::BulkLookupDataT*  blact
	*
	* @return
	*   none
	*/
	void BulkLookupAction(
	    ::identt::utils::SharedTable::pointer stptr,
	    ::identt::query::BulkLookupDataT* blact);

private:
	/** mutex to handle parallel reads */
	std::mutex read_mutex;

	/**
	* LookupGlobal : Service Lookup implementation with GlobalAssoc
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param query
	*   ::identt::query::BulkLookupDataT*  blact
	*
	* @return
	*   none
	*/
	void LookupGlobal(
	    ::identt::utils::SharedTable::pointer stptr,
	    ::identt::query::LookupQueryT* query,
	    ::identt::query::LookupResultT* result);

	/**
	* BulkLookupGlobal : Bulk Lookup Implementation with GlobalAssoc
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param query
	*   ::identt::query::BulkLookupDataT*  blact
	*
	* @param start
	*   size_t start of lookup
	*
	* @param end
	*   size_t end of lookup
	*
	* @return
	*   none
	*/
	void BulkLookupGlobal(
	    ::identt::utils::SharedTable::pointer stptr,
	    ::identt::query::BulkLookupDataT* blact,
	    size_t start, size_t end);

	/**
	* LookupLocal : Service Lookup implementation with LocalAssoc
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param query
	*   ::identt::query::BulkLookupDataT*  blact
	*
	* @return
	*   none
	*/
	void LookupLocal(
	    ::identt::utils::SharedTable::pointer stptr,
	    ::identt::query::LookupQueryT* query,
	    ::identt::query::LookupResultT* result);

	/**
	* BulkLookupLocal : Bulk Lookup Implementation with LocalAssoc
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param query
	*   ::identt::query::BulkLookupDataT*  blact
	*
	* @param start
	*   size_t start of lookup
	*
	* @param end
	*   size_t end of lookup
	*
	* @return
	*   none
	*/
	void BulkLookupLocal(
	    ::identt::utils::SharedTable::pointer stptr,
	    ::identt::query::BulkLookupDataT* blact,
	    size_t start, size_t end);

};
} // namespace query
} // namespace identt

#endif // _IDENTT_STORE_LOOKUP_SERVICE_HPP_
