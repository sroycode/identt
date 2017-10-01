#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

export public_key=${public_key:="xxx"};

## ---- main
${HTTPIE} GET ${TESTURL}/pubkey/ephemeral/isvalid \
	public_key=="${public_key}"
