# -*- coding: utf-8 -*-
#
#  @project identt
#  @file python/utils/HttpClient.py
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
#   HttpClient.py : create key from seed
# 

import unirest
import json

class HttpClient(object):
	def __init__(self):
		pass

	def post_json(self, uri, post_json):
		response = unirest.post(uri, headers ={ "Content-Type": ["application/json"] }, params = json.dumps(post_json) )
		return response

	def post_json_with_secret(self, uri, skey, post_json):
		print json.dumps(post_json)
		response = unirest.post(uri, headers ={"Content-Type": "application/json", "Shared-Secret": skey }, params = json.dumps(post_json) )
		return response

