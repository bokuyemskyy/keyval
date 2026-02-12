file(GLOB SRCS
    src/*.cpp
)

add_library(protocol STATIC ${SRCS})

target_include_directories(protocol PUBLIC include)