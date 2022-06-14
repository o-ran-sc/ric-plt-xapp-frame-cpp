# vim: sw=4 ts=4 noet:
#
#==================================================================================
#   Copyright (c) 2020 Nokia
#   Copyright (c) 2020 AT&T Intellectual Property.
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
#


# For clarity: this generates object, not a lib as the CM command implies.
#
add_library( restapi_client_objects OBJECT restapi_client.cpp)
add_library( restapi_server_objects OBJECT restapi_server.cpp)
target_compile_options(restapi_server_objects PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CXX_FLAGS_TEST}>)
# add_library( restapi_server_objects OBJECT
# 	restapi_server.cpp
# )

target_include_directories (restapi_client_objects PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>
	)

# header files should go into .../include/xfcpp/

if( DEV_PKG )
	install( FILES
		restapi_client.hpp restapi_server.hpp
		DESTINATION ${install_inc}
	)
endif()

