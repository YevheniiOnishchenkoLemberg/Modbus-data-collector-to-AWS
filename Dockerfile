FROM ubuntu:22.04
SHELL ["/bin/bash", "-c"]
ENV SHELL=/bin/bash

RUN apt update
RUN apt install -y g++ gawk \
git libncurses-dev libssl-dev \
python3-distutils rsync unzip zlib1g-dev file wget \
pkg-config cmake

ADD . /project
#WORKDIR HT9_buildroot
