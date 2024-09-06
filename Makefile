# Base image
FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    libgmp-dev \
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

