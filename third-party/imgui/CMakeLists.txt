include_directories("imgui")

file(GLOB_RECURSE IMGUI_SRCS "imgui/*.cpp")

add_library(ImGui ${IMGUI_SRCS})
