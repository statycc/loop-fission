FROM python:3.9.25-bookworm

RUN apt-get update \
    && apt-get install -qqy --no-install-recommends \
       build-essential \
       git \
       bc \
       software-properties-common \
       libjpeg-dev \
       zlib1g-dev \
    && rm -rf /var/lib/apt/lists/* \
    && git clone https://github.com/statycc/loop-fission.git /loop-fission \
    && chmod +x /loop-fission

WORKDIR /loop-fission
RUN pip install --upgrade pip \
    && pip install numpy matplotlib pytablewriter
