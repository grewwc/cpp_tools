file(GLOB SRC_SOURCE ./*.cc)
file(GLOB SRC_HEADER 
    Json.hpp
    fileutils.hpp
)
target_sources(cpp_tools PRIVATE
    ${SRC_SOURCE}
)

file(COPY ${SRC_HEADER} DESTINATION ${CMAKE_SOURCE_DIR}/include)