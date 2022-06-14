# vim: ts=4 sw=4 noet:

#==================================================================================
#	Copyright (c) 2020 Nokia
#	Copyright (c) 2020 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#==================================================================================

# simple makefile to build the examples. This assumes that the xapp framework
# library has been installed or the LD_LIBRARY_PATH and C_INCLUDE_PATH environent
# variables are set to reference the needed files.

%.o:: %.cpp %.hpp
	g++ -g ${prereq%% *} -c

% :: %.cpp
	#C_INCLUDE_PATH=/tmp/usr/include:/usr/local/include g++ $< -g -o $@  -lricxfcpp -lrmr_si -lpthread -lm
	g++ $< -g -o restcpp_xapp -std=c++17 -lricxfcpp -lrmr_si -lpthread -lm -lcpprest -lboost_system -lcrypto -lssl -lpistache
	#g++ $< -g -o $@  /usr/local/lib/libricxfcpp.a -lrmr_si -lpthread -lm

all:: xapp_t2 xapp_t1 rmr_dump xapp_t4
