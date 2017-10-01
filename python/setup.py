# -*- coding: utf-8 -*-
#
#  @project identt
#  @file python/setup.py
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
#   setup.py : setup for mail and sms handler example
# 

import os
from setuptools import setup, find_packages

setup(
    name="MailHandler",
    version="1.0",
    packages=find_packages(exclude=["tests"]),
    description="Reference Synapse Identity Verification and Lookup Server",
    install_requires=[
        "signedjson==1.0.0",
        "unpaddedbase64==1.1.0",
        "service_identity>=1.0.0",
        "pyasn1",
        "pynacl",
        "daemonize",
        "phonenumbers",
        "unirest"
    ],
    setup_requires=[
        "setuptools>=1.0.0"
    ],
    include_package_data=True,
    long_description="Sample Mail and SMS Handler, picks up and sends mails from identt",
)
