/**
 * @project identt
 * @file include/store/StoreBase.hpp
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
 *  StoreBase.hpp :   Shared headers for store
 *
 */
#ifndef _IDENTT_STORE_STOREBASE_HPP_
#define _IDENTT_STORE_STOREBASE_HPP_

#define IDENTT_MEGABYTE 1024 * 1024LL
#define IDENTT_STRN_BLANK ""
#define IDENTT_MIN_TIME 1450000000
#define IDENTT_MAX_TIME 2000000000


#define IDENTT_KEYID_LEN  2

#ifdef IDENTT_USE_BASE80_KEYS
#define IDENTT_NODE_LEN  10
#else
#define IDENTT_NODE_LEN  16
#endif

#define IDENTT_STRING_MAXLEN  1024
#define IDENTT_LEN_NODE_KEY  IDENTT_KEYID_LEN + IDENTT_NODE_LEN

#include <utils/BaseUtils.hpp>
#include "../proto/Store.pb.h"

namespace identt {
namespace store {
class StoreBase {
public:


	/**
	* EncodeKeyType : make prefix key with keytype
	*
	* @param
	*   KeyTypeE keytype
	*
	* @return
	*   std::string Key
	*/
	std::string EncodeKeyType (KeyTypeE keytype);

	/**
	* EncodePrimaryKey : make primary key with id only
	*
	* @param
	*   KeyTypeE keytype
	*
	* @param
	*   ::google::protobuf::uint64 id
	*
	* @return
	*   std::string Key
	*/
	std::string EncodePrimaryKey (KeyTypeE keytype, ::google::protobuf::uint64 id);

	/**
	* DecodePrimaryKey : get keytype and id from primary key
	*
	* @param
	*   std::string& key
	*
	* @return
	*   std::pair<KeyType,uint64_t> keytype, id
	*/
	std::pair<KeyTypeE,uint64_t> DecodePrimaryKey (std::string& key);

	/**
	* EncodeSecondaryKey : make secondary key with one variable only
	*
	* @param
	*   KeyTypeE keytype
	*
	* @param
	*   T key
	*
	* @return
	*   std::string Key
	*/
	template<typename T>
	std::string EncodeSecondaryKey (KeyTypeE keytype, T key);

	/**
	* EncodeSecondaryKey : make secondary key with two variables
	*
	* @param
	*   KeyTypeE keytype
	*
	* @param
	*   T key1
	*
	* @param
	*   U key2
	*
	* @return
	*   std::string Key
	*/
	template<typename T, typename U>
	std::string EncodeSecondaryKey (KeyTypeE keytype, T key1, U key2);

	/**
	* EncodeSecondaryKey : make secondary key with three variables
	*
	* @param
	*   KeyTypeE keytype
	*
	* @param
	*   T key1
	*
	* @param
	*   U key2
	*
	* @param
	*   V key3
	*
	* @return
	*   std::string Key
	*/
	template<typename T, typename U, typename V>
	std::string EncodeSecondaryKey (KeyTypeE keytype, T key1, U key2, V key3);

private:
};
} // namespace store
} // namespace identt
#endif /* _IDENTT_STORE_STOREBASE_HPP_ */
