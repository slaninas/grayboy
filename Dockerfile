FROM ubuntu:20.04

RUN apt update
RUN apt install -y cmake libncurses5-dev clang
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100
RUN update-alternatives --auto c++

Copy . /app
CMD cd /app && mkdir build-docker && cd build-docker && cmake .. && time make -j7 && ctest
