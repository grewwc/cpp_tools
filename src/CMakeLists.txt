file(GLOB SRC_SOURCE ./*.cpp)
file(GLOB SRC_HEADER 
    Json.hpp
)

# 3rt party
file(GLOB GFLAGS_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/gflags/src/*.cc)
file(GLOB CJSON_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/cJSON/*.c)
file(GLOB GLOG_SOURCES 
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/utilities.cc
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/symbolize.cc 
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/demangle.cc
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/logging.cc
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/raw_logging.cc
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/signalhandler.cc
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/vlog_is_on.cc
    ${CMAKE_SOURCE_DIR}/thirdparty/glog/src/dangle.cc
)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/cJSON)

target_sources(cpp_tools PRIVATE
    ${SRC_SOURCE}
    ${GFLAGS_SOURCES}
    ${GLOG_SOURCES}
    ${CJSON_SOURCES}
)

file(COPY ${SRC_HEADER} DESTINATION ${CMAKE_SOURCE_DIR}/include)
