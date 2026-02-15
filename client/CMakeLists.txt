FetchContent_Declare(
  replxx
  GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(replxx)

file(GLOB SRCS
    src/*.cpp
)

add_executable(keyval-cli ${SRCS})

target_include_directories(keyval-cli PRIVATE include)

target_link_libraries(keyval-cli PRIVATE protocol socketpoll replxx::replxx)