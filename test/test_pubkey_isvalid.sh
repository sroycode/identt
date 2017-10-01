#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export public_key=${public_key:="+MGScogHF3SysbmYDoQ5fFGwj3maf9+LwozjoVnq4cw"};

## ---- main
${HTTPIE} GET ${TESTURL}/pubkey/isvalid \
	public_key=="${public_key}"
