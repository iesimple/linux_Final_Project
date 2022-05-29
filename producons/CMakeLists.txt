cmake_minimum_required(VERSION 3.5)
project(sem)

# 设置输出可执行文件路径, PROJECT_SOURCE_DIR是工程根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(ALL_SOURCE_FILES producons.c)

add_executable(producons ${ALL_SOURCE_FILES})

target_link_libraries(producons rt pthread)


