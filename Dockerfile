# Use an official base image
FROM ubuntu:20.04

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    libgmp-dev \
    libssl-dev \
    wget \
    curl \
    git

# Copy the source code into the container
COPY . /usr/src/btccollider

# Set the working directory
WORKDIR /usr/src/btccollider

# Run make to build the binary
RUN make

# Define the default command
CMD ["./BTCCollider"]
