/**
 * @project identt
 * @file src/query/JsonElem.cc
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
 *  JsonElem.cc :  JsomElem implementation
 *
 */

#include <query/JsonElem.hpp>

/**
* Constructor : string
*
*/
identt::query::JsonElem::JsonElem(std::string name, std::string value)
	: nam(name), val(value), typ(IDENTT_JE_STRING) {}

/**
* Constructor : float
*
*/
identt::query::JsonElem::JsonElem(std::string name, float value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_DOUBLE) {}

/**
* Constructor : float
*
*/
identt::query::JsonElem::JsonElem(const char* name, float value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_DOUBLE) {}

/**
* Constructor : double
*
*/
identt::query::JsonElem::JsonElem(std::string name, double value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_DOUBLE) {}

/**
* Constructor : double
*
*/
identt::query::JsonElem::JsonElem(const char* name, double value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_DOUBLE) {}

/**
* Constructor : int32_t
*
*/
identt::query::JsonElem::JsonElem(std::string name, int32_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : int32_t
*
*/
identt::query::JsonElem::JsonElem(const char* name, int32_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : uint32_t
*
*/
identt::query::JsonElem::JsonElem(std::string name, uint32_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : uint32_t
*
*/
identt::query::JsonElem::JsonElem(const char* name, uint32_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : int64_t
*
*/
identt::query::JsonElem::JsonElem(std::string name, int64_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : int64_t
*
*/
identt::query::JsonElem::JsonElem(const char* name, int64_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : uint64_t
*
*/
identt::query::JsonElem::JsonElem(std::string name, uint64_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : uint64_t
*
*/
identt::query::JsonElem::JsonElem(const char* name, uint64_t value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_INT64) {}

/**
* Constructor : bool
*
*/
identt::query::JsonElem::JsonElem(std::string name, bool value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_BOOL) {}

/**
* Constructor : bool
*
*/
identt::query::JsonElem::JsonElem(const char* name, bool value)
	: nam(name), val(std::to_string(value)), typ(IDENTT_JE_BOOL) {}

identt::query::JsonElem::~JsonElem() {}

