cmake_minimum_required(VERSION 3.0)
project(Pi-File-Server)

#Require pthreads
find_package(Threads REQUIRED)

#Compile with C++11, -Wall, and -g
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

#Include commands .cpp's
file(GLOB commandCPPs RELATIVE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"Commands/*.cpp"
)

#Find the cpp files in Commands
execute_process(COMMAND ls ../Pi-File-Server/ OUTPUT_FILE COMMAND_FOLDER_LS)
execute_process(COMMAND grep \\.cpp$ INPUT_FILE COMMAND_FOLDER_LS OUTPUT_VARIABLE COMMAND_NAMES)
set(GEN_FNS "${COMMAND_NAMES}")

#Format the commands into an array of 
#function pointers to their 'generate' functions
string(REPLACE "\n" "" GEN_FNS "${GEN_FNS}")
string(REGEX REPLACE ".cpp$" "::generate\n\n};" GEN_FNS "${GEN_FNS}")
string(REPLACE ".cpp" "::generate(),\n" GEN_FNS "${GEN_FNS}")
set(GEN_FNS "Command (*cmdsArr[]) (std::string s) = {\n\n${GEN_FNS}")

#Format the command names into an array
string(REGEX REPLACE ".cpp$" "\"\n\n};" COMMAND_NAMES "${COMMAND_NAMES}")

#Make the cpp file
file(WRITE Autogen.cpp "//Match file index to Command generate function.\n")
file(APPEND Autogen.cpp "\n\n\n")
file(APPEND Autogen.cpp "//Match file index to Command name.\n")




#execute_process(COMMAND ls ../Pi-File-Server/ OUTPUT_FILE Autogen.hpp)

#[[
#Create a.out
add_executable(a.out
	Pi-File-Server/main.cpp
	Pi-File-Server/Server.cpp
	Pi-File-Server/FileHandler.cpp 
	${commandCPPs} )

#Link pthreads
target_link_libraries( a.out ${CMAKE_THREAD_LIBS_INIT} )
]]
