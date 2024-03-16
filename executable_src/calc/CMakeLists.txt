add_executable(c main.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    j PRIVATE cpp_tools
)

