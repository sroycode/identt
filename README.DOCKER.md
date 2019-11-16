# Running with Docker

This starts the identt container ( as master ) 
The default identt.conf should work , however you may want to change the `shared_secret` for production


1. With localhost access assuming port 9091 9092 are free
```
docker run --name=my_identt sroycode/identt bash /usr/local/bin/run_identt.sh
```

1. On a standalone network with persistent data
```
docker network create my_identt
export RHOME=/tmp/data ## wherever u want the data to do
docker run -p 9091:9091 -p 9092:9092 \
-v ${RHOME}:/data --network=my_identt --name=my_identt \
sroycode/identt bash /usr/local/bin/run_identt.sh
```

