set(BUILD_TESTING OFF)
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

add_subdirectory(gflags)

file(GLOB GFLAGS_SOURCES gflags/src/*.cc)
file(GLOB GLOG_SOURCES 
    glog/src/utilities.cc
    glog/src/symbolize.cc 
    glog/src/demangle.cc
    glog/src/logging.cc
)

target_sources(cpp_tools PRIVATE 
    ${GFLAGS_SOURCES}
    ${GLOG_SOURCES}
)

find_package(Threads REQUIRED)
include_directories(cpp_tools  ${Threads_INCLUDE_DIRS})
target_link_libraries(cpp_tools PRIVATE Threads::Threads)

target_include_directories(cpp_tools PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(cpp_tools PUBLIC ${CMAKE_SOURCE_DIR}/include/glog)

