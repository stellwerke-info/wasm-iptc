FROM ubuntu:noble

USER root
WORKDIR /root

# configure system
RUN apt-get update -qq && \
	DEBIAN_FRONTEND="noninteractive" apt-get upgrade -y && \
    ln -sf /usr/share/zoneinfo/UTC /etc/localtime && \
    DEBIAN_FRONTEND="noninteractive" apt-get -y install vim wget curl python-is-python3 python3-pip make binutils git clang lld libc-dev && \
    rm -rf /var/lib/apt/lists/*

ENV LC_ALL="en_US.utf8" \
    LANGUAGE="en_US.utf8" \
    LANG="en_US.utf8"

RUN mkdir /work && chown ubuntu:ubuntu /work && chmod 755 /work

USER ubuntu
WORKDIR /work
ENV HOME="/work/home"

CMD /bin/bash
