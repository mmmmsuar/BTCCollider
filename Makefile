# Use the Ubuntu base image
FROM ubuntu:20.04

# Set environment variables for CUDA installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies including CUDA
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    libgmp-dev \
    git \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Install CUDA from NVIDIA repository
RUN apt-get update && apt-get install -y \
    gnupg2 \
    software-properties-common \
    && wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin \
    && mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600 \
    && wget https://developer.download.nvidia.com/compute/cuda/11.4.2/local_installers/cuda-repo-ubuntu2004-11-4-local_11.4.2-1_amd64.deb \
    && dpkg -i cuda-repo-ubuntu2004-11-4-local_11.4.2-1_amd64.deb \
    && apt-key add /var/cuda-repo-ubuntu2004-11-4-local/7fa2af80.pub \
    && apt-get update && apt-get install -y cuda

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
