set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SRCS
    ${SRC_DIR}/*.cpp
)

add_executable(server ${SRCS})

target_include_directories(server
    PRIVATE
        ${SRC_DIR}
)

target_link_libraries(server PRIVATE common)

target_compile_features(server PRIVATE cxx_std_23)
