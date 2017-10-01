/**
 * @project identt
 * @file include/store/ValidateService.hpp
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
 *  ValidateService.hpp : Header for email msisdn service
 *
 */
#ifndef _IDENTT_STORE_VALIDATE_SERVICE_HPP_
#define _IDENTT_STORE_VALIDATE_SERVICE_HPP_

#include <query/QueryBase.hpp>
#include <store/StoreBase.hpp>

namespace identt {
namespace store {

class ValidateService : public StoreBase {
public:


	/**
	* RequestTokenAction : Service Endpoint RequestToken
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param rtoka
	*   ::identt::query::RequestTokenDataT* rtoka
	*
	* @return
	*   none
	*/
	void RequestTokenAction(
	    ::identt::utils::SharedTable::pointer stptr, ::identt::query::RequestTokenDataT* rtoka);

	/**
	* SubmitTokenAction : Service Endpoint SubmitToken
	*
	* @param stptr
	*   ::identt::utils::SharedTable::pointer stptr
	*
	* @param stoka
	*   ::identt::query::SubmitTokenDataT* stoka
	*
	* @return
	*   none
	*/
	void SubmitTokenAction(
	    ::identt::utils::SharedTable::pointer stptr, ::identt::query::SubmitTokenDataT* stoka);

private:
	/**
	* PopulateAddress : Validate RequestToken medium and populate address
	*
	* @param reqtok
	*   ::identt::query::RequestTokenT* reqtok
	*
	* @param medium
	*   const std::string& medium
	*
	* @param address
	*   std::string& address to populate
	*
	* @return
	*   none
	*/
	void PopulateAddress( ::identt::query::RequestTokenT* reqtok, const std::string& medium, std::string& address);
};
} // namespace store
} // namespace identt

#endif // _IDENTT_STORE_VALIDATE_SERVICE_HPP_

