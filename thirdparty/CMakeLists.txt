
add_subdirectory(gflags)
# add_subdirectory(cJson)
add_subdirectory(curl-master)
# add_subdirectory(grpc)
# file(GLOB GFLAGS_SOURCES gflags/src/*.cc)


message(STATUS "${output_var}")


file(GLOB CJSON_HEADERS cJSON/*.h)
# file(GLOB CJSON_SOURCES cJSON/*.c)
file(GLOB THREAD_POOL_HEADERS thread-pool/include/BS_thread_pool.hpp)

message("copy cjson file")
file(COPY ${CJSON_HEADERS} DESTINATION ${CMAKE_SOURCE_DIR}/include)
file(COPY ${THREAD_POOL_HEADERS} DESTINATION ${CMAKE_SOURCE_DIR}/include)

# file(GLOB GLOG_SOURCES 
#     glog/src/utilities.cc
#     glog/src/symbolize.cc 
#     glog/src/demangle.cc
#     glog/src/logging.cc
#     glog/src/raw_logging.cc
#     glog/src/signalhandler.cc
#     glog/src/vlog_is_on.cc
#     glog/src/dangle.cc
# )

# target_sources(cpp_tools PRIVATE 
#     ${GFLAGS_SOURCES}
#     ${GLOG_SOURCES}
#     ${CJSON_SOURCES}
# )

find_package(Threads REQUIRED)
include_directories(cpp_tools  ${Threads_INCLUDE_DIRS})
target_link_libraries(cpp_tools PRIVATE Threads::Threads)

target_include_directories(cpp_tools PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(cpp_tools PUBLIC ${CMAKE_SOURCE_DIR}/include/glog)

