
add_subdirectory(gflags)

file(GLOB SOURCES gflags/src/*.cc)

target_sources(cpp_tools PRIVATE ${SOURCES})
message(INFO ">>>>>> what ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Threads REQUIRED)
include_directories(cpp_tools  ${Threads_INCLUDE_DIRS})
target_link_libraries(cpp_tools PRIVATE Threads::Threads)

target_include_directories(cpp_tools PUBLIC ${CMAKE_SOURCE_DIR}/include)

