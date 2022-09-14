# syntax=docker/dockerfile:1
FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt update -y
RUN apt install -y build-essential git bc

RUN git clone https://github.com/statycc/loop-fission.git
WORKDIR loop-fission

RUN apt install -y software-properties-common
RUN add-apt-repository ppa:deadsnakes/ppa
RUN apt install -y python3.9
RUN apt install -y python3.9-distutils
RUN apt install -y python3-pip
RUN apt install -y libjpeg-dev zlib1g-dev

RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.9 1
RUN update-alternatives --set python3 /usr/bin/python3.9

RUN pip3 install --upgrade pip setuptools wheel
RUN pip3 install -r requirements.txt
RUN mkdir eval