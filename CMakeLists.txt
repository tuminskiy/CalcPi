cmake_minimum_required(VERSION 3.15.0)
project(CalcPi)

add_executable(CalcPi)

set_target_properties(CalcPi PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

set(SRC main.cpp)

target_sources(CalcPi PRIVATE ${SRC})