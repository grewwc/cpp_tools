add_executable(tt main.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    tt PRIVATE cpp_tools
)

