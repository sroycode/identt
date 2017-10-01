/**
 * @project identt
 * @file src/crypto/CryptoBase.cc
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
 *  CryptoBase.cc : Base implementation for  Sign keys
 *
 */
#include <crypto/CryptoBase.hpp>
#include <query/bin2ascii.h>
#include <sodium/utils.h>

/**
* CryptoBase : constructor
*
*/
identt::crypto::CryptoBase::CryptoBase () : scope(0) {}

/**
* GetAlgo : get key algo
*
*/
const std::string identt::crypto::CryptoBase::GetAlgo()
{
	throw identt::BadCodeException("Base class CryptoBase should never be called");
}

/**
* GetSeed : return seed of private key
*
*/
std::string identt::crypto::CryptoBase::GetSeed() const
{
	throw identt::BadCodeException("Not Implemented");
}

/**
* GetPublicKey : return public key
*
*/
std::string identt::crypto::CryptoBase::GetPublicKey() const
{
	return b64_encode_strip(pubkey);
}

/**
* GetSecretKey : return private key
*
*/
std::string identt::crypto::CryptoBase::GetSecretKey() const
{
	return b64_encode_strip(seckey);
}

/**
* GetSignature : return signature
*
*/
std::string identt::crypto::CryptoBase::GetSignature(const std::string& input) const
{
	throw identt::BadCodeException("Not Implemented");
}

/**
* VerifySignature : return signature
*
*/
bool identt::crypto::CryptoBase::VerifySignature(const std::string& signature, const std::string& input) const
{
	throw identt::BadCodeException("Not Implemented");
}

/**
* Encode : return encoded
*
*/
bool identt::crypto::CryptoBase::Encode(const std::string& input, std::string& output) const
{
	throw identt::BadCodeException("Not Implemented");
}

/**
* Decode : return decoded
*
*/
bool identt::crypto::CryptoBase::Decode(const std::string& input, std::string& output) const
{
	throw identt::BadCodeException("Not Implemented");
}
