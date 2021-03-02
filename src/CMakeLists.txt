#定义一个SRC_LIST变量 包含该目录下所有文件夹
aux_source_directory(. SRC_LIST)

add_executable(ConnectDBPool ${SRC_LIST})
target_link_libraries(ConnectDBPool pthread mysqlclient)
