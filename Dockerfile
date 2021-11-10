FROM ubuntu:20.04

RUN apt update
RUN apt install -y cmake libncurses5-dev g++-10 time
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-10 10

Copy . /app
CMD cd /app && mkdir build-docker && cd build-docker && cmake .. && /usr/bin/time make -j7 && ctest

