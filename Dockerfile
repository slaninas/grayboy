FROM ubuntu:22.04

# Prevent being stuck at timezone selection
ENV TZ=Europe/Kiev
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt update
RUN apt install -y cmake g++ time libsdl2-dev

Copy . /app
CMD cd /app && mkdir build-docker && cd build-docker && cmake .. && time make -j7 && ctest
