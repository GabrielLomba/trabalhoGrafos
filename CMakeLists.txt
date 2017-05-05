cmake_minimum_required(VERSION 3.7)
project(trabalhoGrafos)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Grafo.h Grafo.cpp No.h No.cpp Aresta.h Aresta.cpp Clock.h Clock.cpp)
add_executable(trabalhoGrafos ${SOURCE_FILES})