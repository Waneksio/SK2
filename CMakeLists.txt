cmake_minimum_required(VERSION 3.14)
project(sk2)

set(CMAKE_CXX_STANDARD 17)

add_executable(sk2 main.cpp Player.cpp Player.h Entity.cpp Position.h Position.cpp Position.h Entity.cpp Entity.h Food.cpp Food.h)