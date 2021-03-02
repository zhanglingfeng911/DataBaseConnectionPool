cmake_minimum_required(VERSION 3.0)
project(commonconnpool)#项目名字

set(CMKAE_BUILD_TYPE "Debug")#生成debug版本 进行gdb调试
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)#设置可执行文件的输出路径
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(src)