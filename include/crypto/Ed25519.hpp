/**
 * @project identt
 * @file include/crypto/Ed25519.hpp
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
 *  Ed25519.hpp : Library for Ed25519
 *
 */
#ifndef _IDENTT_CRYPTO_ED25519_HPP_
#define _IDENTT_CRYPTO_ED25519_HPP_

#include <crypto/CryptoBase.hpp>

namespace identt {
namespace crypto {
class Ed25519 : virtual public CryptoBase {
public:

	/**
	* GetAlgo : get key algo
	*
	* @return
	*   const std::string algo
	*/
	const static std::string GetAlgo();

	/**
	* Ed25519 : constructor
	*
	*/
	Ed25519();

	/**
	* Ed25519 : constructor with seed
	*
	* @param seed
	*   std::string& seed
	*
	*/
	explicit Ed25519(const std::string& seed);

	/**
	* Ed25519 : constructor with 2 keys
	*
	* @param pubkey_
	*   std::string& public key
	*
	* @param seckey_
	*   std::string& secret key
	*
	*/
	explicit Ed25519(const std::string& pubkey_, const std::string& seckey_);

	/**
	* Ed25519 : constructor with scope and 1 key for verification only
	*
	* @param scope_
	*   unsigned int scope
	*
	* @param onekey_
	*   std::string& verification or decoding key
	*
	*/
	explicit Ed25519( unsigned int scope_, const std::string& onekey_);


	Ed25519(const Ed25519&) = delete;
	Ed25519& operator=(const Ed25519&) = delete;


	/**
	* GetSeed : return seed of private key
	*
	* @return
	*   std::string
	*/
	virtual std::string GetSeed() const override;

	/**
	* GetSignature : return signature
	*
	* @param
	*   const std::string& input
	*
	* @return
	*   std::string
	*/
	virtual std::string GetSignature(const std::string& input) const override;

	/**
	* VerifySignature : return signature valid status
	*
	* @param
	*   const std::string& signature
	*
	* @param
	*   const std::string& input
	*
	* @return
	*   bool status
	*/
	virtual bool VerifySignature(const std::string& signature, const std::string& input) const override;

	/**
	* Encode : return encoded
	*
	* @param
	*   const std::string& input
	*
	* @param
	*   std::string& output
	*
	* @return
	*   bool success
	*/
	virtual bool Encode(const std::string& input, std::string& output) const override;

	/**
	* Decode : return decoded
	*
	* @param
	*   const std::string& input
	*
	* @param
	*   std::string& output
	*
	* @return
	*   bool success
	*/
	virtual bool Decode(const std::string& input, std::string& output) const override;

protected:

};
} // namespace crypto
} // namespace identt
#endif /* _IDENTT_CRYPTO_ED25519_HPP_ */
