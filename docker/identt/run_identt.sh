#!/bin/bash

MY_IP=`awk 'END{print $1}' /etc/hosts`

export LIBASYNC_NUM_THREADS=${LIBASYNC_NUM_THREADS:=8}
IDENTT_CONF=${IDENTT_CONF:="/usr/local/etc/identt.conf"}
SLAVEOF=""
if [ $# -eq 1 ] ; then
	SLAVEOF="$1"
fi

mkdir -p /data/logs /data/identt_data /data/identt_logs
identtserver -config ${IDENTT_CONF} $SLAVEOF
