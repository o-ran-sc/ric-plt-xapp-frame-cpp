set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
# BUILD TYPE
message("A ${CMAKE_BUILD_TYPE} build configuration is detected")
message("CPP flag is  ${CMAKE_CXX_FLAGS} ")
message("CPP standard is  ${CMAKE_CXX_STANDARD} ")

include_directories( "${srcd}/src/rest-client")
#SOURCE FILES
file(GLOB SOURCE_FILES  "${srcd}/src/rest-client/*.cpp" )


add_library(rest-client_objects OBJECT  ${SOURCE_FILES})

target_include_directories (rest-client_objects PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# header files should go into .../include/xfcpp/

file(GLOB SOURCE_INSTALL_FILES  "${srcd}/src/rest-client/*.h")
#set( install_inc_client "include/ricxfcpp/rest-client" )

if( DEV_PKG )
        install( FILES
                 ${SOURCE_INSTALL_FILES}
                DESTINATION ${install_inc}
        )
endif()


