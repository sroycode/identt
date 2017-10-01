#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export mailkey_ed25519_sk="1UfbvW0r7HYk2tavQW+Iy4E8JSYpESIlTR7GxeFzqQmq774C26xP74VUeq9B09cmWA+3F6jrvyBdFwZ+dMuoqQ"
export mailhost="identt.doesntexist.com"
. ${SRCDIR}/config.sh

## ---- main
./identt_getmail --key "$mailkey_ed25519_sk" --mailhost "$mailhost"
