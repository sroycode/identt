#!/bin/bash

VENV_PATH=venv
SSL_PATH=/usr/local/opt/openssl

virtualenv $VENV_PATH
source ${VENV_PATH}/bin/activate
pip install --upgrade pip
pip install --upgrade setuptools
pip install unpaddedbase64
pip install signedjson
pip install pyasn1
pip install pynacl
pip install daemonize
pip install unirest
# hack for openssl on mac os x using homebrew
if [ `uname` = "Darwin" ] ; then
pip install cryptography --global-option=build_ext --global-option="-L${SSL_PATH}/lib" --global-option="-I${SSL_PATH}/include"
else
pip install cryptography
fi
python setup.py install
