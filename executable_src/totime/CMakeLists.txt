add_executable(tp main.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    tp PRIVATE cpp_tools
)

