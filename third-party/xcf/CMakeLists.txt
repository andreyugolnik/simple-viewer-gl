include_directories("xcftools")

file(GLOB_RECURSE XCFTOOLS_SRCS "xcftools/*.cpp")

add_library(XcfTools ${XCFTOOLS_SRCS})
