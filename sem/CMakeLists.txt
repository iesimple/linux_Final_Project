cmake_minimum_required(VERSION 3.5)
project(sem)

# 设置输出可执行文件路径, PROJECT_SOURCE_DIR是工程根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

add_executable(producons producons.c)

target_link_libraries(producons rt pthread)


