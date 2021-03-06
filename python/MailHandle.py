# -*- coding: utf-8 -*-
#
#  @project identt
#  @file python/sendmail/sendmail.py
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
#   mailhandle.py : setup for mail handle
# 

import ConfigParser
import logging
import os

from utils.SydentEd25519 import SydentEd25519
from utils.HttpClient import HttpClient

import urllib2

logger = logging.getLogger(__name__)

class MailHandle:
	CONFIG_SECTIONS = ['general', 'http', 'crypto', 'mail', 'sms']
	CONFIG_DEFAULTS = {
		'server.name': 'mailhandle',
		'log.path': '.',
		'pidfile.path': 'mailhandle.pid',
		'http.endpoint': 'http://localhost:9091/_identt/identity/api/v1/getmailstosend',
		'token.length': '6',
		'crypto.shared_secret' : 'qu++AtusT++FVHqvQdPXJlgPtxeo678gXRcGfnTLqKk'
	}

	def __init__(self):
		logger.info("Starting MailHandle")
		self.parse_config()

		logPath = self.cfg.get('general', "log.path")
		logging.basicConfig(level=logging.INFO, filename=logPath)
		self.pidfile = self.cfg.get('general', "pidfile.path");

		self.server_name = self.cfg.get('general', 'server.name')
		self.endpoint = self.cfg.get('general', 'http.endpoint')
		self.shared_secret = self.cfg.get('crypto', 'shared_secret')

	def parse_config(self):
		self.cfg = ConfigParser.SafeConfigParser(MailHandle.CONFIG_DEFAULTS)
		for sect in MailHandle.CONFIG_SECTIONS:
			try:
				self.cfg.add_section(sect)
			except ConfigParser.DuplicateSectionError:
				pass
		self.cfg.read("mailhandle.conf")

	def run(self):
		if self.pidfile:
			with open(self.pidfile, 'w') as pidfile:
				pidfile.write(str(os.getpid()) + "\n")

		json_data = {
			'lastid':0,
			'limit':10
		}
		try:
			httpc = HttpClient()
			response = httpc.post_json_with_secret(self.endpoint,self.shared_secret,json_data)
			print response.body
		except urllib2.HTTPError, e:
			print e.args
		except urllib2.URLError, e:
			print e.reason


if __name__ == '__main__':
	syd = MailHandle()
	syd.run()
