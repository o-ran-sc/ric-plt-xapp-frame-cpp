# ==================================================================================
# Copyright (c) 2020 HCL Technologies Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==================================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
# BUILD TYPE
message("A ${CMAKE_BUILD_TYPE} build configuration is detected")
message("CPP flag is  ${CMAKE_CXX_FLAGS} ")
message("CPP standard is  ${CMAKE_CXX_STANDARD} ")

include_directories( "${srcd}/src/model")
#SOURCE FILES
file(GLOB SOURCE_FILES  "${srcd}/src/model/*.cpp" )


add_library(model_objects OBJECT  ${SOURCE_FILES})

target_include_directories (model_objects PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# header files should go into .../include/xfcpp/

file(GLOB SOURCE_INSTALL_FILES  "${srcd}/src/model/*.h")
#set( install_inc_client "include/ricxfcpp/rest-client" )

if( DEV_PKG )
        install( FILES
                 ${SOURCE_INSTALL_FILES}
                DESTINATION ${install_inc}
        )
endif()


