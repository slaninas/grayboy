FROM ubuntu:20.04

RUN apt update
RUN apt install -y cmake libncurses5-dev g++ time libsdl2-dev

Copy . /app
CMD cd /app && mkdir build-docker && cd build-docker && cmake .. && time make -j7 && ctest
