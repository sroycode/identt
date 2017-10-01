/**
 * @project identt
 * @file include/query/JsonElem.hpp
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
 *  JsonElem.hpp :  Headers for JsomElem
 *
 */
#ifndef _IDENTT_QUERY_JSONELEM_HPP_
#define _IDENTT_QUERY_JSONELEM_HPP_

#include <string>

namespace identt {
namespace query {

struct JsonElem {
	const std::string nam;
	const std::string val;
	const enum JsonElemE {
		IDENTT_JE_STRING = 0,
		IDENTT_JE_DOUBLE = 1,
		IDENTT_JE_INT64 = 2,
		IDENTT_JE_BOOL = 3
	} typ;

	/**
	* Constructor : string
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   std::string value to populate
	*
	*/
	JsonElem(std::string name, std::string value);

	/**
	* Constructor : float
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   float value to populate
	*
	*/
	explicit JsonElem(std::string name, float value);

	/**
	* Constructor : float
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   float value to populate
	*
	*/
	explicit JsonElem(const char* name, float value);

	/**
	* Constructor : double
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   double value to populate
	*
	*/
	explicit JsonElem(std::string name, double value);

	/**
	* Constructor : double
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   double value to populate
	*
	*/
	explicit JsonElem(const char* name, double value);

	/**
	* Constructor : int32_t
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   int32_t value to populate
	*
	*/
	explicit JsonElem(std::string name, int32_t value);

	/**
	* Constructor : int32_t
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   int32_t value to populate
	*
	*/
	explicit JsonElem(const char* name, int32_t value);


	/**
	* Constructor : uint32_t
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   std::string value to populate
	*
	*/
	explicit JsonElem(std::string name, uint32_t value);

	/**
	* Constructor : uint32_t
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   const char* value to populate
	*
	*/
	explicit JsonElem(const char* name, uint32_t value);

	/**
	* Constructor : int64_t
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   int64_t value to populate
	*
	*/
	explicit JsonElem(std::string name, int64_t value);

	/**
	* Constructor : int64_t
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   int64_t value to populate
	*
	*/
	explicit JsonElem(const char* name, int64_t value);

	/**
	* Constructor : uint64_t
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   std::string value to populate
	*
	*/
	explicit JsonElem(std::string name, uint64_t value);

	/**
	* Constructor : uint64_t
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   const char* value to populate
	*
	*/
	explicit JsonElem(const char* name, uint64_t value);

	/**
	* Constructor : bool
	*
	* @param name
	*   std::string name
	*
	* @param value
	*   bool value to populate
	*
	*/
	explicit JsonElem(std::string name, bool value);

	/**
	* Constructor : bool
	*
	* @param name
	*   const char* name
	*
	* @param value
	*   bool value to populate
	*
	*/
	explicit JsonElem(const char* name, bool value);

	/**
	* Constructor deleted
	*
	*/
	explicit JsonElem() = delete;
	explicit JsonElem(std::string , std::string , JsonElemE) = delete;


	virtual ~JsonElem();

};

}
}

#endif
