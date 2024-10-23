add_executable(his main.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    his PRIVATE cpp_tools
)

