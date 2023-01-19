cmake_minimum_required (VERSION 3.8)

project("macos_hid_detect")

add_executable(${PROJECT_NAME} "src/main.c" "src/helpers.c" "src/constants.c")
# add_compile_options(-fconstant-cfstrings)

target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit" "-framework CoreFoundation" "-framework AppKit")
target_include_directories(${PROJECT_NAME} PRIVATE "include")
