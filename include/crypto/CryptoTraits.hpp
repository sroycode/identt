/**
 * @project identt
 * @file include/crypto/CryptoTraits.hpp
 * @author  S Roychowdhury <sroycode AT gmail DOT com>
 * @version 1.0.0
 *
 * @section LICENSE
 *
 * Copyright(c) 2017 S Roychowdhury.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files(the "Software"), to deal in
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
 *  CryptoTraits.hpp : Traits file for all Crypto
 *
 */
#ifndef _IDENTT_CRYPTO_CRYPTO_TRAITS_HPP_
#define _IDENTT_CRYPTO_CRYPTO_TRAITS_HPP_

#include <memory>
#include <crypto/Ed25519.hpp>

namespace identt {
namespace crypto {
class CryptoTraits {
public:
	using pointer = std::shared_ptr<CryptoBase>;

	/**
	* Create : create new keys
	*
	* @param ctype
	*   const std::string key algo
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer Create(const std::string ctype)
	{
		if (ctype==Ed25519::GetAlgo()) return std::make_shared<Ed25519>();
		throw ::identt::BadCodeException("Bogus Algo");
	}

	/**
	* CreateFromSeed : create from seed
	*
	* @param ctype
	*   const std::string key algo
	*
	* @param seed
	*   const std::string& seed
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer CreateFromSeed(const std::string ctype, const std::string& seed)
	{
		if (ctype==Ed25519::GetAlgo()) return std::make_shared<Ed25519>(seed);
		throw ::identt::BadCodeException("Bogus Algo");
	}

	/**
	* Create : create single purpose key
	*
	* @param ctype
	*   const std::string key algo
	*
	* @param scope
	*   unsigned int scope
	*
	* @param key
	*   const std::string key
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer Create(const std::string ctype, unsigned int scope, const std::string& key)
	{
		if (ctype==Ed25519::GetAlgo()) return std::make_shared<Ed25519>(scope,key);
		throw ::identt::BadCodeException("Bogus Algo");
	}

	/**
	* Create : only operator
	*
	* @param ctype
	*   const std::string key algo
	*
	* @param pkey
	*   const std::string& public key
	*
	* @param skey
	*   const std::string& private key
	*
	* @return
	*   std::shared_ptr<CryptoBase>
	*
	*/
	static pointer Create(const std::string ctype, const std::string& pkey, const std::string& skey)
	{
		if (ctype==Ed25519::GetAlgo()) return std::make_shared<Ed25519>(pkey,skey);
		throw ::identt::BadCodeException("Bogus Algo");
	}

};
} // namespace crypto
} // namespace identt
#endif /* _IDENTT_CRYPTO_CRYPTO_TRAITS_HPP_ */
