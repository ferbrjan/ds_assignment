cmake_minimum_required (VERSION 2.8.11)
project(assignment1)

INCLUDE_DIRECTORIES("include")
#INCLUDE_DIRECTORIES("gnu")

add_executable(server apps/server.c src/lines.c)

find_package (Threads)
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})

find_package(Java REQUIRED)
include(UseJava)

add_jar(client apps/client.java MANIFEST manifest.mf)
