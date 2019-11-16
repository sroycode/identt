# Docker File for Ubuntu based identt

FROM ubuntu:bionic

MAINTAINER S Roychowdhury <sroycode@gmail.com>

ENV DEBIAN_FRONTEND noninteractive
ENV LANG C.UTF-8

RUN apt-get -y update -qq && \
    apt-get -y install locales apt-utils software-properties-common && \
    locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    add-apt-repository ppa:exonum/rocksdb && \
    add-apt-repository ppa:maarten-fonville/protobuf && \
    add-apt-repository ppa:xapian-backports/ppa && \
    apt-get update -y -qq && \
    apt-get install -y \
    --no-install-recommends \
    git wget ccache cmake make libtool pkg-config g++ gcc autoconf automake curl jq lcov \
    protobuf-compiler vim-common libboost-all-dev libboost-all-dev \
    libcurl4-openssl-dev zlib1g-dev liblz4-dev libprotobuf-dev \
    libgoogle-glog-dev libgflags-dev libgoogle-perftools-dev libsnappy-dev libbz2-dev libz-dev libctemplate-dev \
    libtbb-dev libzstd-dev libsodium-dev libosmium-dev librocksdb-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /tmp/* /var/tmp/*

WORKDIR /app

#ENV MVERSION devel
ENV MVERSION v0.52

RUN git clone https://github.com/sroycode/identt.git \
	&& cd identt \
	&& git checkout ${MVERSION} \
	&& mkdir -p build templates \
	&& cd build \
	&& cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install

COPY identt.conf /usr/local/etc/identt.conf
COPY run_identt.sh /usr/local/bin/run_identt.sh

EXPOSE 9091
EXPOSE 9092
