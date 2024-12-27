add_executable(cj main.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    cj PRIVATE cpp_tools
)

