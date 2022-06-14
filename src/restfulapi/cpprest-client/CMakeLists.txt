cmake_minimum_required (VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CPPREST-CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

#SUPPORTING FILES
set(SUPPORTING_FILES  "ApiClient" "ApiConfiguration" "ApiException"
    "HttpContent" "IHttpBody" "JsonBody" "ModelBase" "MultipartFormData"
    "Object")
#SOURCE FILES
file(GLOB SOURCE_FILES  "api/*"  "model/*")
file(GLOB API_HEADER_FILES  "api/*.h")
file(GLOB MODEL_HEADER_FILES "model/*.h")
file(GLOB API_BASE_FILES  "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

include_directories(${CPPREST-CLIENT_DIR} "${CPPREST-CLIENT_DIR}/api" "${CPPREST-CLIENT_DIR}/model")

add_library(cpprest-client_objects OBJECT ${SUPPORTING_FILES} ${SOURCE_FILES})

# header files should go into .../include/xfcpp/
if( DEV_PKG )
    install( FILES ${API_BASE_FILES} DESTINATION ${install_inc}/cpprest-client)
    install( FILES ${API_HEADER_FILES} ${MODEL_HEADER_FILES} DESTINATION ${install_inc}/cpprest-client/api)
endif()

