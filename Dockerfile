FROM ubuntu:22.04

# Prevent being stuck at timezone selection
ENV TZ=Europe/Kiev
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt update
RUN apt install -y cmake time libsdl2-dev

RUN apt install -y lsb-release wget software-properties-common
RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-14 20
RUN apt install -y clang-tidy-14

Copy . /app
RUN cd /app && mkdir build-docker && cd build-docker && cmake -DCMAKE_BUILD_TYPE=Debug .. && time make -j7
