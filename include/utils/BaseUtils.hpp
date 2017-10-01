/**
 * @project identt
 * @file include/utils/BaseUtils.hpp
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
 *  BaseUtils.hpp :   For Standard Inclusion
 *
 */
#ifndef _IDENTT_UTILS_BASEUTILS_HPP_
#define _IDENTT_UTILS_BASEUTILS_HPP_

#include <glog/logging.h>

#include <string>
#include <memory>
#include <exception>

#define IDENTT_MAX_LAT 90.0
#define IDENTT_MIN_LAT -90.0
#define IDENTT_MAX_LON 180.0
#define IDENTT_MIN_LON -180.0

namespace identt {
class JdException : public std::exception {
protected:
	std::string msg_;
	int err_;
public:
	explicit JdException() : msg_(""), err_(0) {}
	explicit JdException(const char* msg) : msg_(msg), err_(0) {}
	explicit JdException(std::string& msg) : msg_(msg), err_(0) {}
	explicit JdException(std::string msg) : msg_(msg), err_(0) {}
	explicit JdException(const char* msg, int err) : msg_(msg),err_(err) {}
	explicit JdException(std::string& msg, int err) : msg_(msg),err_(err) {}
	explicit JdException(std::string msg, int err) : msg_(msg),err_(err) {}
	virtual ~JdException() throw () {}
	virtual const char* what() const throw ()
	{
		return msg_.c_str();
	}
	virtual int ecode() const throw ()
	{
		return err_;
	}
};

class InitialException : public JdException {
	using JdException::JdException;
};
class ConfigException : public JdException {
	using JdException::JdException;
};
class NotFoundException : public JdException {
	using JdException::JdException;
};
class BadQueryException : public JdException {
	using JdException::JdException;
};
class BadDataException : public JdException {
	using JdException::JdException;
};
class BadJsonException : public JdException {
	using JdException::JdException;
};
class BadCodeException : public JdException {
	using JdException::JdException;
};


} // namespace identt
#endif // _IDENTT_UTILS_BASEUTILS_HPP_
