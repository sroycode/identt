#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export shared_secret=${shared_secret:="qu++AtusT++FVHqvQdPXJlgPtxeo678gXRcGfnTLqKk"}

## ---- main
./identt_getmail --shared_secret "$shared_secret"
