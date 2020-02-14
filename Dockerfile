FROM ubuntu:18.10
ARG boost_libs
ARG boost_libs=none

# Prepare build environment
RUN apt-get update && \
    apt-get -qq -y install gcc g++ \
    cmake curl wget pkg-config \
    libtool python3 python3-pip

RUN pip3 install conan && \
    conan remote add stiffstream https://api.bintray.com/conan/stiffstream/public && \
    conan remote add public-conan https://api.bintray.com/conan/bincrafters/public-conan

RUN mkdir cse280-countr
COPY *.cpp cse280-countr
COPY conanfile.txt cse280-countr
COPY CMakeLists.txt cse280-countr

RUN echo "*** Building an example ***" \
	&& cd cse280-countr \
	&& mkdir build \
	&& cd build \
	&& conan install -o restinio:boost_libs=$boost_libs --build=missing ..  \
	&& cmake .. \
	&& cmake --build . --config Release

EXPOSE 8080

WORKDIR /cse280-countr/build/bin

CMD ./countr_server

