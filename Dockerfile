FROM gcc:latest

RUN apt-get update; \
    apt-get -y upgrade
RUN apt-get -y install g++ cmake git subversion autoconf automake libtool curl make g++ unzip wget

RUN g++ --version

RUN wget https://github.com/google/protobuf/releases/download/v3.4.0/protobuf-cpp-3.4.0.zip
RUN unzip protobuf-cpp-3.4.0.zip
WORKDIR protobuf-3.4.0
RUN ls
RUN ./autogen.sh
RUN ./configure
RUN make -j8
RUN make install
RUN ldconfig



WORKDIR /app
COPY . /app



RUN find . -iwholename '*cmake*' -not -name CMakeLists.txt -delete
RUN ls
RUN protoc -I=. --cpp_out=. trie.proto
RUN cmake .

RUN make clean
RUN make
RUN ls
RUN ./protobuf_trie