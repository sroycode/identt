/**
 * @project identt
 * @file include/http/Crypto.hpp
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
 *  Crypto.hpp :  for WS Server ( Modified from https
 *
 */
#ifndef _IDENTT_HTTP_CRYPTO_HPP_
#define _IDENTT_HTTP_CRYPTO_HPP_

#include <string>
#include <cmath>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

namespace identt {
namespace http {

class Crypto {
public:
	class Base64 {
	public:
		static std::string encode(const std::string &ascii)
		{
			std::string base64;

			BIO *bio, *b64;
			BUF_MEM *bptr=BUF_MEM_new();

			b64 = BIO_new(BIO_f_base64());
			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
			bio = BIO_new(BIO_s_mem());
			BIO_push(b64, bio);
			BIO_set_mem_buf(b64, bptr, BIO_CLOSE);

			//Write directly to base64-buffer to avoid copy
			int base64_length=static_cast<int>(round(4*ceil((double)ascii.size()/3.0)));
			base64.resize(base64_length);
			bptr->length=0;
			bptr->max=base64_length+1;
			bptr->data=(char*)&base64[0];

			BIO_write(b64, &ascii[0], static_cast<int>(ascii.size()));
			BIO_flush(b64);

			//To keep &base64[0] through BIO_free_all(b64)
			bptr->length=0;
			bptr->max=0;
			bptr->data=nullptr;

			BIO_free_all(b64);

			return base64;
		}

		static std::string decode(const std::string &base64)
		{
			std::string ascii;

			//Resize ascii, however, the size is a up to two bytes too large.
			ascii.resize((6*base64.size())/8);
			BIO *b64, *bio;

			b64 = BIO_new(BIO_f_base64());
			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
			bio = BIO_new_mem_buf((char*)&base64[0], static_cast<int>(base64.size()));
			bio = BIO_push(b64, bio);

			int decoded_length = BIO_read(bio, &ascii[0], static_cast<int>(ascii.size()));
			ascii.resize(decoded_length);

			BIO_free_all(b64);

			return ascii;
		}
	};

	static std::string MD5(const std::string &input, size_t iterations=1)
	{
		std::string hash;

		hash.resize(128 / 8);
		::MD5(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		for (size_t c = 1; c < iterations; ++c)
			::MD5(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		return hash;
	}

	static std::string SHA1(const std::string &input, size_t iterations=1)
	{
		std::string hash;

		hash.resize(160 / 8);
		::SHA1(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		for (size_t c = 1; c < iterations; ++c)
			::SHA1(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		return hash;
	}

	static std::string SHA256(const std::string &input, size_t iterations=1)
	{
		std::string hash;

		hash.resize(256 / 8);
		::SHA256(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		for (size_t c = 1; c < iterations; ++c)
			::SHA256(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		return hash;
	}

	static std::string SHA512(const std::string &input, size_t iterations=1)
	{
		std::string hash;

		hash.resize(512 / 8);
		::SHA512(reinterpret_cast<const unsigned char*>(&input[0]), input.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		for (size_t c = 1; c < iterations; ++c)
			::SHA512(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(), reinterpret_cast<unsigned char*>(&hash[0]));

		return hash;
	}
};
} // namespace http
} // namespace identt
#endif	/* _IDENTT_HTTP_CRYPTO_HPP_ */
