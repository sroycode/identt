# Configuration

## Section system

- accesslog : access log for daemon identtserver
- errorlog : error log for daemon identtserver
- baseurl : base URL to append to callback to this instance, this may be load-balanced/proxied and ssl offloading might happen external
- hostname : server host name
- shared_secret : key to access _identt/ functions
- lookup_requires_key : if set to greater than 0 lookup function needs accesskey
- invite_requires_key : if set to greater than 0 invite function needs accesskey
- dont_send_sms : if greater than 0 sms is not sent 
- bulk_parallel_lookups : if greater than 0 then parallel lookups are done in chunks
- lookup_uses_local : if greater than zero localassoc cache is used for lookups

## Section work

- datadir : location of rocksdb database
- cachesize : rocksdb cachesize in MB
- logdatadir : location of log database , - ignored if IDENTT_USE_SEPARATE_LOGDB not set
- logcachesize : log database cachesize in MB , - ignored if IDENTT_USE_SEPARATE_LOGDB not set
- hostseed_ed25519 : seed for this instance host key

## Section http

- host : HTTP host for internal HTTP host
- port : HTTP port

## Section hrpc 

- host : intermachine host for internal HTTP host
- port : intermachine port
- thisurl : URL to reach this service from other machines in cluster
