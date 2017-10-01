# -*- coding: utf-8 -*-
#
#  @project identt
#  @file python/utils/SydentEd25519.py
#  @author  S Roychowdhury <sroycode AT gmail DOT com>
#  @version 1.0.0
# 
#  @section LICENSE
# 
#  Copyright (c) 2017 S Roychowdhury.
# 
#  Permission is hereby granted, free of charge, to any person obtaining a copy of
#  this software and associated documentation files (the "Software"), to deal in
#  the Software without restriction, including without limitation the rights to
#  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
#  the Software, and to permit persons to whom the Software is furnished to do so,
#  subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
#  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
#  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
#  @section DESCRIPTION
# 
#   SydentEd25519.py : create key from seed
# 

import nacl.encoding
import nacl.signing
import nacl.exceptions
import signedjson.key
import signedjson.sign

from unpaddedbase64 import encode_base64
from unpaddedbase64 import decode_base64

class SydentEd25519:
	def __init__(self, seed):
		myseed = decode_base64(seed)
		self.signing_key = nacl.signing.SigningKey(myseed)
		self.signing_key.version = 'mail'
		self.signing_key.alg = signedjson.key.NACL_ED25519

	def AddSign(self, host_name, tosign):
		myret = signedjson.sign.sign_json(tosign, host_name, self.signing_key)
		return myret
