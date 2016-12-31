cmake_minimum_required(VERSION 3.0)
project(Pi-File-Server)

#Require pthreads
find_package(Threads REQUIRED)

#Compile with C++11, -Wall, and -g
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

#Create a.out
add_executable(a.out
	Pi-File-Server/main.cpp
	Pi-File-Server/Server.cpp
	Pi-File-Server/FileHandler.cpp
	Pi-File-Server/Synchronized.cpp )

#Link pthreads
target_link_libraries( a.out ${CMAKE_THREAD_LIBS_INIT} )

