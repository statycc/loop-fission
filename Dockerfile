# syntax=docker/dockerfile:1
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update -y \
    && apt install -y build-essential git bc \
    && apt install -y software-properties-common \
    && add-apt-repository ppa:deadsnakes/ppa \
    && apt install -y python3.9 \
    && apt install -y python3.9-distutils \
    && apt install -y python3-pip \
    && apt install -y libjpeg-dev zlib1g-dev

RUN git clone https://github.com/statycc/loop-fission.git
WORKDIR loop-fission

RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.9 1 \
    && update-alternatives --set python3 /usr/bin/python3.9

RUN pip3 install --upgrade pip setuptools wheel
RUN pip3 install -r requirements.txt
RUN mkdir eval