# Configuration

## Section system

- accesslog : access log for daemon identtserver
- errorlog : error log for daemon identtserver

## Section work

- datadir : location of rocksdb database
- cachesize : rocksdb cachesize in MB
- logdatadir : location of log database , required but ignored if IDENTT_USE_SEPARATE_LOGDB not set
- logcachesize : log database cachesize in MB
- base_url : base URL to append to callback to this instance
- hostseed_ed25519 : seed for this instance host key
- mail_host : mail fetch program hostname
- mailkey_ed25519 : mail fetch program public key
- listen : host-port this instance master listens at ( if master )
- tohear : host-port this instance master is reachable at from slaves ( if master )
- master : master of the instance at start ( overwritten by commandline )

## Section http

- host : HTTP host 
- port : HTTP port
