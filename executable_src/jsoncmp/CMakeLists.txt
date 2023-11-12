add_executable(j main.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    j PRIVATE cpp_tools
)

file(COPY ${CMAKE_SOURCE_DIR}/build/executable_src/jsoncmp/j DESTINATION ${CMAKE_SOURCE_DIR}/bin)