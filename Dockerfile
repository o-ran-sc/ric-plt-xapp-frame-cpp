# vim: ts=4 sw=4 noet:
#==================================================================================
#	Copyright (c) 2018-2019 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#	   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#==================================================================================


# --------------------------------------------------------------------------------------
#	Mnemonic:	Dockerfile
#	Abstract:	This can be used to create a base environment for using the xAPP
#				framework.  It will install RMR and the framework libraries. It also
#				installs make and g++ so that it can be used as a builder environment.
#
#				The unit tests are executed as a part of the build process; if they are
#				not passing then the build will fail.
#
#				Building should be as simple as:
#
#					docker build -f Dockerfile -t ricxfcpp:[version]
#
#	Date:		23 March 2020
#	Author:		E. Scott Daniels
# --------------------------------------------------------------------------------------


FROM nexus3.o-ran-sc.org:10002/o-ran-sc/bldr-ubuntu20-c-go:1.0.0 as buildenv
RUN mkdir /playpen

RUN apt-get update --fix-missing &&  apt-get install -y cmake gcc make git g++ wget \
		libcpprest-dev rapidjson-dev meson libssl-dev

RUN mkdir /playpen/bin /playpen/factory /playpen/factory/src /playpen/factory/test
ARG SRC=.

WORKDIR /playpen
# Install RMr (runtime and dev) from debian package cached on packagecloud.io
ARG RMR_VER=4.8.0

# if package cloud is actually working, this is preferred
#
#RUN wget -nv --content-disposition https://packagecloud.io/o-ran-sc/staging/packages/debian/stretch/rmr_${RMR_VER}_amd64.deb/download.deb
#RUN wget -nv --content-disposition https://packagecloud.io/o-ran-sc/staging/packages/debian/stretch/rmr-dev_${RMR_VER}_amd64.deb/download.deb
#RUN dpkg -i rmr_${RMR_VER}_amd64.deb
#RUN dpkg -i rmr-dev_${RMR_VER}_amd64.deb
#
# else this:
#
COPY ${SRC}/build_rmr.sh /playpen/bin
RUN bash /playpen/bin/build_rmr.sh -t ${RMR_VER}

RUN git clone https://github.com/pistacheio/pistache.git && cd pistache && meson setup build \
    --buildtype=release \
    -DPISTACHE_USE_SSL=true \
    -DPISTACHE_BUILD_EXAMPLES=false \
    -DPISTACHE_BUILD_TESTS=false \
    -DPISTACHE_BUILD_DOCS=false \
    --prefix="$PWD/prefix" \
	 && meson install -C build && \
	 cp -rf prefix/include/pistache /usr/local/include/ && \
	 cp -rf prefix/lib/x86_64-linux-gnu/* /usr/lib/x86_64-linux-gnu/

#copy the content as git repo inside the container.
#COPY ${SRC}/CMakeLists.txt /playpen/factory/
#COPY ${SRC}/src /playpen/factory/src/
#COPY ${SRC}/test /playpen/factory/test/
COPY ${SRC}/examples /tmp/examples/
COPY . /playpen/factory


# Go to the factory and build our stuff
#
ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib
ENV C_INCLUDE_PATH=/usr/local/include
RUN cd /playpen/factory; rm -fr .build; mkdir .build; cd .build; cmake .. -DDEV_PKG=1; make install; cmake .. -DDEV_PKG=0; make install




#
# Run unit tests will come after building process
#
COPY ${SRC}/test/* /playpen/factory/test/
RUN cd /playpen/factory/test; bash unit_test.sh

# -----  final, smaller image ----------------------------------
FROM ubuntu:20.04

# must add compile tools to make it a builder environmnent. If a build environment isn't needed 
# comment out the next line and reduce the image size by more than 180M.
#
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN apt-get update && apt-get install -y --no-install-recommends make g++ libssl-dev tzdata libboost-all-dev libcpprest-dev

# if bash doesn't cut it for run_replay grab a real shell and clean up as much as we can
RUN apt-get update; apt-get install -y ksh
RUN rm -fr /var/lib/apt/lists

RUN mkdir -p /usr/local/include/ricxfcpp
COPY --from=buildenv /usr/local/lib /usr/local/lib/
COPY --from=buildenv /usr/local/include/ricxfcpp /usr/local/include/ricxfcpp/
COPY --from=buildenv /usr/local/include/rmr /usr/local/include/rmr/
COPY --from=buildenv /usr/local/include/pistache /usr/local/include/pistache
COPY --from=buildenv /usr/lib/x86_64-linux-gnu/libpistache* /usr/lib/x86_64-linux-gnu/

ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib
ENV C_INCLUDE_PATH=/usr/local/include
WORKDIR /factory

CMD [ "make" ]
