# Adapted from git@github.com:sol-prog/Clang-in-Docker.git

# Use the latest stable Ubuntu version
FROM ubuntu:16.04

# Copy over relevant files
COPY . /chit-cpp/

# Install libraries and programs
RUN apt update && apt install -y \
	xz-utils \
	build-essential \
	clang++-5.0 \
	make \
	libasio-dev \
	#libboost-all-dev \
	vim-gtk

# Start from a Bash prompt
CMD [ "/bin/bash" ]
