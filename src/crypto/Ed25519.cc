/**
 * @project identt
 * @file src/crypto/Ed25519.cc
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
 *  Ed25519.cc : Ed25519 for Key Signs
 *
 */
#include <crypto/Ed25519.hpp>
#include <sodium/crypto_sign_ed25519.h>

#include <query/bin2ascii.h>
#include <sodium/utils.h>

/**
* GetAlgo : get key algo
*
*/
const std::string identt::crypto::Ed25519::GetAlgo()
{
	return "ed25519";
}

/**
* Ed25519 : constructor
*
*/
identt::crypto::Ed25519::Ed25519()
{
	pubkey.resize(crypto_sign_ed25519_PUBLICKEYBYTES);
	seckey.resize(crypto_sign_ed25519_SECRETKEYBYTES);
	crypto_sign_ed25519_keypair(
	    reinterpret_cast<unsigned char*>(&pubkey[0]),
	    reinterpret_cast<unsigned char*>(&seckey[0])
	);
	scope=IDENTT_CRYPTO_SCOPE_ALL;
}

/**
* Ed25519 : constructor with seed
*
*/
identt::crypto::Ed25519::Ed25519 (const std::string& seed)
	: CryptoBase()
{
	const std::string mseed = b64_sanit_decode(seed);
	if (mseed.length() != crypto_sign_ed25519_SEEDBYTES)
		throw identt::BadDataException("Bad Seed for Key");
	pubkey.resize(crypto_sign_ed25519_PUBLICKEYBYTES);
	seckey.resize(crypto_sign_ed25519_SECRETKEYBYTES);
	int stat = crypto_sign_ed25519_seed_keypair(
	               reinterpret_cast<unsigned char*>(&pubkey[0]),
	               reinterpret_cast<unsigned char*>(&seckey[0]),
	               reinterpret_cast<const unsigned char*>(&mseed[0])
	           );
	if (stat<0)
		throw identt::BadDataException("Bad Seed for Key");
	scope=IDENTT_CRYPTO_SCOPE_ALL;
}

/**
* Ed25519 : constructor with 2 keys
*
*/
identt::crypto::Ed25519::Ed25519 (const std::string& pubkey_, const std::string& seckey_)
	: CryptoBase()
{
	pubkey = b64_sanit_decode(pubkey_);
	seckey = b64_sanit_decode(seckey_);
	if (pubkey.length() != crypto_sign_ed25519_PUBLICKEYBYTES)
		throw identt::BadDataException("Bad Public Key");
	if (seckey.length() != crypto_sign_ed25519_SECRETKEYBYTES)
		throw identt::BadDataException("Bad Secret Key");
	scope=IDENTT_CRYPTO_SCOPE_ALL;
}

/**
* Ed25519 : constructor with 1 key
*
*/
identt::crypto::Ed25519::Ed25519 (unsigned int scope_, const std::string& onekey_)
	: CryptoBase()
{
	if (scope_ & IDENTT_CRYPTO_SCOPE_ENCODE) {
		scope = IDENTT_CRYPTO_SCOPE_ENCODE;
		seckey = b64_sanit_decode(onekey_);
	} else if (scope_ & IDENTT_CRYPTO_SCOPE_DECODE) {
		scope = IDENTT_CRYPTO_SCOPE_DECODE;
		pubkey = b64_sanit_decode(onekey_);
	} else if (scope_ & IDENTT_CRYPTO_SCOPE_SIGNIT) {
		scope = IDENTT_CRYPTO_SCOPE_SIGNIT;
		seckey = b64_sanit_decode(onekey_);
	} else if (scope_ & IDENTT_CRYPTO_SCOPE_VERIFY) {
		scope = IDENTT_CRYPTO_SCOPE_VERIFY;
		pubkey = b64_sanit_decode(onekey_);
	} else {
		throw identt::BadCodeException("Undefined constructor for ED25519");
	}
}

/**
* GetSeed : return seed of private key
*
*/
std::string identt::crypto::Ed25519::GetSeed() const
{
	if (!(this->scope & IDENTT_CRYPTO_SCOPE_ALL))
		throw identt::BadCodeException("This class cannot sign so no seed");
	std::string seed;
	seed.resize(crypto_sign_ed25519_SEEDBYTES);
	crypto_sign_ed25519_sk_to_seed(
	    reinterpret_cast<unsigned char*>(&seed[0]),
	    reinterpret_cast<const unsigned char*>(&seckey[0])
	);
	return b64_encode_strip(seed);
}

/**
* GetSignature : return signature
*
*/
std::string identt::crypto::Ed25519::GetSignature(const std::string& input) const
{
	if (!(this->scope & IDENTT_CRYPTO_SCOPE_SIGNIT))
		throw identt::BadCodeException("This class cannot sign");
	std::string output(crypto_sign_ed25519_BYTES,' '); // blank fixed length key
	unsigned long long output_length = output.length();
	crypto_sign_ed25519_detached(
	    reinterpret_cast<unsigned char*>(&output[0]),
	    NULL, // &output_length,
	    reinterpret_cast<const unsigned char*>(&input[0]), input.length(),
	    reinterpret_cast<const unsigned char*>(&seckey[0])
	);
	return b64_encode_strip(output);
}

/**
* VerifySignature : return signature
*
*/
bool identt::crypto::Ed25519::VerifySignature(const std::string& signature, const std::string& input) const
{
	if (!(this->scope & IDENTT_CRYPTO_SCOPE_VERIFY))
		throw identt::BadCodeException("This class cannot verify");
	const std::string decoded=b64_sanit_decode(signature);
	auto status = crypto_sign_ed25519_verify_detached(
	                  reinterpret_cast<const unsigned char*>(&decoded[0]),
	                  reinterpret_cast<const unsigned char*>(&input[0]), input.length(),
	                  reinterpret_cast<const unsigned char*>(&pubkey[0])
	              );
	return (status==0);
}

/**
* Encode : return encoded
*
*/
bool identt::crypto::Ed25519::Encode(const std::string& input, std::string& output) const
{
	throw identt::BadCodeException("Not Implemented");
}

/**
* Decode : return decoded
*
*/
bool identt::crypto::Ed25519::Decode(const std::string& input, std::string& output) const
{
	throw identt::BadCodeException("Not Implemented");
}
