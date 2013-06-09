cmake_minimum_required(VERSION 2.6)
project(MLAT C)

ADD_DEFINITIONS(-g -O2 -Wall -Wextra)

ADD_EXECUTABLE(mlat mlat.c)

TARGET_LINK_LIBRARIES(mlat m)
