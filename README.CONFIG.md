# Configuration

## Section system

- accesslog : access log for daemon identtserver
- errorlog : error log for daemon identtserver
- baseurl : base URL to append to callback to this instance, this may be load-balanced/proxied and ssl offloading might happen external
- hostname : server host name
- shared_secret : key to access _identt/ functions
- master : master of the instance at start ( overwritten by commandline )

## Section work

- datadir : location of rocksdb database
- logdatadir : location of log database , required but ignored if IDENTT_USE_SEPARATE_LOGDB not set
- cachesize : rocksdb cachesize in MB
- logcachesize : log database cachesize in MB
- hostseed_ed25519 : seed for this instance host key

## Section http

- host : HTTP host 
- port : HTTP port
