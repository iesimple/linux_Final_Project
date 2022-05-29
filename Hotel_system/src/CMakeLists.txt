include_directories(../include)

set(SOURCE_FILE main.c systemInit.h systemInit.c)
set(COMMON_HEADERS ../include/common_types.h)

add_executable(main ${SOURCE_FILE} ${COMMON_HEADERS})
target_link_libraries(main rt pthread)