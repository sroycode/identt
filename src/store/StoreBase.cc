/**
 * @project identt
 * @file src/store/StoreBase.cc
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
 *  StoreBase.cc : Store Base Implementation
 *
 */
#include <iomanip>
#include <sstream>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <store/StoreBase.hpp>

#define IDENTT_MACRO_HEX(A)       std::setbase(16) << std::setfill ('0') << std::setw ( A )
#define IDENTT_MACRO_STR(A)       std::setfill (' ') << std::setw ( A )

#define IDENTT_FMT_ID_KEY(A)      IDENTT_MACRO_HEX(IDENTT_KEYID_LEN) << A

#ifdef IDENTT_USE_BASE80_KEYS
#define IDENTT_FMT_NODE_ONLY(A)   IDENTT_MACRO_STR(IDENTT_NODE_LEN) << Base80String<uint64_t,IDENTT_NODE_LEN>(A)
#else
#define IDENTT_FMT_NODE_ONLY(A)   IDENTT_MACRO_HEX(IDENTT_NODE_LEN) << A
#endif

#define IDENTT_FMT_NODE_KEY(A,B)  IDENTT_FMT_ID_KEY(A) << IDENTT_FMT_NODE_ONLY(B)

// #define IDENTT_FMT_SEP "\x1E"
#define IDENTT_FMT_SEP ":"
/* UINT is defined here so can be changed independent of node if needed */
#define IDENTT_FMT_UINT_ONLY(A)    IDENTT_FMT_NODE_ONLY(A)
/* STR is defined here so can be changed to hash in future if needed */
#define IDENTT_FMT_STRN_ONLY(A)    A
/* SINT is defined here so can be changed independent of node if needed */
#define IDENTT_FMT_SINT_ONLY(A)   IDENTT_MACRO_HEX(IDENTT_NODE_LEN) << A

#define IDENTT_FMT_UINT_KEY(A,B)   IDENTT_FMT_ID_KEY(A) << IDENTT_FMT_UINT_ONLY(B)
#define IDENTT_FMT_STRN_KEY(A,B)   IDENTT_FMT_ID_KEY(A) << IDENTT_FMT_STRN_ONLY(B)


/**
* Base80String : Get limited char string
*  : someone needs to get a faster way for this to be usable
*  refer http://www.hackersdelight.org/divcMore.pdf
*
*/
template<typename T, size_t N>
inline std::string Base80String(T dividend)
{
	const static char usechars[] = "()+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_abcdefghijklmnopqrstuvwxyz";
	T modulo=0;
	std::string out(N,' '); // N  (char 32 space)
	size_t c=N;
	while (dividend > 0 && c>0) {
		modulo = (dividend - 1) % 80;
		dividend = (T)((dividend - modulo) / 80);
		out[--c] = usechars[modulo];
	}
	return out;
}

/**
* Str2Uint64 : Convert the first 8 characters of a string to 64 bit
*
* @param str
*   std::string& string to change
*
* @return
*   uint64_t hash
*/
inline uint64_t Str2Uint64 (std::string str)
{
	boost::algorithm::to_upper(str);
	uint64_t t=0;
	size_t counter=8;
	for (char& i : str) {
		t+= std::pow(i,counter);
		if (--counter==0) break;
	}
	return t;
}

/**
* EncodeKeyType : make prefix key with keytype
*
*/
std::string identt::store::StoreBase::EncodeKeyType(identt::store::KeyTypeE keytype)
{
	std::stringstream ss;
	ss << IDENTT_FMT_ID_KEY( (unsigned short)keytype );
	return ss.str();
}

/**
* EncodePrimaryKey : make primary key with id only
*
*/
std::string identt::store::StoreBase::EncodePrimaryKey(identt::store::KeyTypeE keytype, ::google::protobuf::uint64 id)
{
	std::stringstream ss;
	ss << IDENTT_FMT_NODE_KEY( (unsigned short)keytype, id );
	return ss.str();
}

/**
* DecodePrimaryKey : get keytype and id from primary key
*
*/
std::pair<identt::store::KeyTypeE,uint64_t> identt::store::StoreBase::DecodePrimaryKey (std::string& key)
{

	if (key.length() < ( IDENTT_KEYID_LEN + IDENTT_NODE_LEN ))
		throw identt::BadDataException("Invalid Key probed");
	identt::store::KeyTypeE x = (identt::store::KeyTypeE) std::stoull(key.substr(0,IDENTT_KEYID_LEN),0,16);
	uint64_t y = std::stoull(key.substr(IDENTT_KEYID_LEN,IDENTT_NODE_LEN),0,16);
	return std::make_pair(x,y);
}

namespace identt {
namespace store {

/** 1 value */

/**
* EncodeSecondaryKey : make secondary key with uint64_t
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key );
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key );
	return ss.str();
}

/** 2 value */

/**
* EncodeSecondaryKey : make secondary key with two keys uint64_t , uint64_t
*  : u-u unique , u non unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, uint64_t key2)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key2);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with two keys uint64_t , std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, std::string key2)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key2);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with two keys std::string , uint64_t
*  : always unique
*  : s-u unique , s non unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key1, uint64_t key2)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key2);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with two keys std::string , std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key1, std::string key2)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key2);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with two keys uint64_t, int32_t
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, int32_t key2)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_SINT_ONLY(key2);
	return ss.str();
}

/** 3 value */

/**
* EncodeSecondaryKey : make secondary key with three keys uint64_t , uint64_t , uint64_t
*  : u-u-u unique , u-u non unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, uint64_t key2, uint64_t key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys uint64_t , uint64_t , std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, uint64_t key2, std::string key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys uint64_t , std::string , uint64_t
*  : always unique
*  : u-s-u unique , u-s non unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, std::string key2, uint64_t key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys uint64_t , std::string , std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, uint64_t key1, std::string key2, std::string key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_UINT_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys std::string , uint64_t , uint64_t
*  : s-u-u unique , s-u non unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key1, uint64_t key2, uint64_t key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys std::string , uint64_t , std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key1, uint64_t key2, std::string key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys std::string , std::string , uint64_t
*  : s-s-u unique , s-s non unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key1, std::string key2, uint64_t key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_UINT_ONLY(key3);
	return ss.str();
}

/**
* EncodeSecondaryKey : make secondary key with three keys std::string , std::string , std::string
*  : always unique
*
*/
template<> std::string StoreBase::EncodeSecondaryKey (KeyTypeE keytype, std::string key1, std::string key2, std::string key3)
{
	std::stringstream ss;
	ss << IDENTT_FMT_STRN_KEY( (unsigned short)keytype, key1)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key2)
	   << IDENTT_FMT_SEP << IDENTT_FMT_STRN_ONLY(key3);
	return ss.str();
}


} // namespace store
} // namespace identt
