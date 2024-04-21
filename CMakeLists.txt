cmake_minimum_required(VERSION 3.25)
project(Project_Draft)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project_Draft main.cpp DataBlock.cpp DataBlock.h Ram.cpp Ram.h Cache.cpp Cache.h Address.cpp Address.h Cpu.cpp Cpu.h)
