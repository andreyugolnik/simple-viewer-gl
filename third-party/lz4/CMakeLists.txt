include_directories(include/lz4)

file(GLOB LZ4_SRCS "src/*.c")

add_library(LZ4 STATIC ${LZ4_SRCS})
target_include_directories(LZ4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
