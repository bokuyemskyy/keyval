set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SRCS
    ${SRC_DIR}/*.cpp
)

add_library(common STATIC ${SRCS})

target_include_directories(common
    PUBLIC include
)

target_compile_features(common PUBLIC cxx_std_23)
