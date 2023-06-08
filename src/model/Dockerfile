FROM nexus3.o-ran-sc.org:10001/ubuntu:18.04
RUN apt-get update && apt-get install -y cmake

RUN apt-get install -y libcpprest-dev

RUN apt-get install -y  g++ git libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev libwebsocketpp-dev openssl libssl-dev ninja-build zlib1g-dev
RUN git clone https://github.com/Microsoft/cpprestsdk.git casablanca && \
    cd casablanca && \
    mkdir build && \
    cd build && \
    cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_SAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    ninja && \
    ninja install
RUN git clone https://github.com/nlohmann/json.git && cd json && cmake . && make install
RUN git clone https://github.com/pboettch/json-schema-validator.git && cd json-schema-validator && git checkout cae6fad80001510077a7f40e68477a31ec443add &&mkdir build &&cd build && cmake .. && make install
COPY . ./
