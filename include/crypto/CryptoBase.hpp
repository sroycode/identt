/**
 * @project identt
 * @file include/crypto/CryptoBase.hpp
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
 *  CryptoBase.hpp :
 *
 */
#ifndef _IDENTT_CRYPTO_CRYPTO_BASE_HPP_
#define _IDENTT_CRYPTO_CRYPTO_BASE_HPP_

// #define IDENTT_CRYPTO_B64_STRIP(X) while (X.back()=='=') X.pop_back();
// #define IDENTT_CRYPTO_B64_PAD(X) if( 3 - ((X.length() + 3) % 4) ) X.resize((3 - ((X.length() + 3) % 4))+X.length(),'=');

#define IDENTT_CRYPTO_SCOPE_ENCODE         0x0001
#define IDENTT_CRYPTO_SCOPE_DECODE         0x0002
#define IDENTT_CRYPTO_SCOPE_SIGNIT         0x0004
#define IDENTT_CRYPTO_SCOPE_VERIFY         0x0008
#define IDENTT_CRYPTO_SCOPE_ENCODE_DECODE  IDENTT_CRYPTO_SCOPE_ENCODE        + IDENTT_CRYPTO_SCOPE_DECODE
#define IDENTT_CRYPTO_SCOPE_SIGNIT_VERIFY  IDENTT_CRYPTO_SCOPE_SIGNIT        + IDENTT_CRYPTO_SCOPE_VERIFY
#define IDENTT_CRYPTO_SCOPE_ALL            IDENTT_CRYPTO_SCOPE_ENCODE_DECODE + IDENTT_CRYPTO_SCOPE_SIGNIT_VERIFY

#include <utils/BaseUtils.hpp>

namespace identt {
namespace crypto {
class CryptoBase {
public:

	/**
	* GetAlgo : get key algo
	*
	* @return
	*   const std::string algo
	*/
	const static std::string GetAlgo();

	/**
	* CryptoBase : constructor
	*
	*/
	CryptoBase();

	/**
	* CryptoBase : constructor
	*
	*/
	CryptoBase(const CryptoBase&) = delete;
	CryptoBase& operator=(const CryptoBase&) = delete;

	/**
	* GetSeed : return seed of private key
	*
	* @return
	*   std::string
	*/
	virtual std::string GetSeed() const;

	/**
	* GetPublicKey : return public key
	*
	* @return
	*   std::string
	*/
	virtual std::string GetPublicKey() const;

	/**
	* GetSecretKey : return private key
	*
	* @return
	*   std::string
	*/
	virtual std::string GetSecretKey() const;

	/**
	* GetSignature : return signature
	*
	* @param
	*   const std::string& input
	*
	* @return
	*   std::string
	*/
	virtual std::string GetSignature(const std::string& input) const;

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
	virtual bool VerifySignature(const std::string& signature, const std::string& input) const;

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
	virtual bool Encode(const std::string& input, std::string& output) const;

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
	virtual bool Decode(const std::string& input, std::string& output) const;

protected:
	unsigned int scope;
	std::string pubkey;
	std::string seckey;

};
} // namespace crypto
} // namespace identt
#endif /* _IDENTT_CRYPTO_CRYPTO_BASE_HPP_ */
