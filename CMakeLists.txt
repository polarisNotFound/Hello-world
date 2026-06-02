cmake_minimum_required(VERSION 3.21)
project(A02_easyX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 静态编译（关键：让 exe 可以复制到任何电脑运行）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")

# ====================== 必须先写这一行！======================
add_executable(${PROJECT_NAME} main.cpp)

# EasyX 路径
set(EasyX_DIR ${CMAKE_SOURCE_DIR}/third_party/easyX)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${EasyX_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
        ${EasyX_DIR}/lib64
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        easyx
        winmm
        gdi32
        user32
)