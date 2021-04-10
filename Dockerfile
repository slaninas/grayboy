FROM ubuntu:20.04
Copy . /app

RUN apt update
RUN apt install -y cmake libncurses5-dev g++-10 time
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-10 10
RUN cd /app && mkdir build && cd build && cmake .. && time make -j6

CMD cd /app/build && time ctest

