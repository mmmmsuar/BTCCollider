# Base image
FROM nvidia/cuda:11.4.2-cudnn8-devel-ubuntu20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    libgmp-dev \
    libsecp256k1-dev \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install secp256k1 library
RUN git clone https://github.com/bitcoin-core/secp256k1.git /usr/src/secp256k1
WORKDIR /usr/src/secp256k1
RUN ./autogen.sh && ./configure --enable-module-recovery && make && make install

# Clone the BTCCollider repo
WORKDIR /usr/src/
COPY . /usr/src/BTCCollider
WORKDIR /usr/src/BTCCollider

# Compile BTCCollider
RUN make clean && make

# Command to run the program
CMD ["./BTCCollider"]
