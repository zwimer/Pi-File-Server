# -------------------------------Setup-------------------------------


cmake_minimum_required(VERSION 3.0)
project(Pi-File-Server)

#Require pthreads
find_package(Threads REQUIRED)

#Compile with C++11, -Wall, and -g
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")


# --------------------Create Auto-generated files--------------------


#Auto generated file
set(OUTFILE "AutoGen")

#Path to Commands
set(CMD_PATH "../Pi-File-Server/")

#Find the cpp and hpp files in Commands
execute_process(COMMAND ls "${CMD_PATH}" OUTPUT_FILE COMMAND_FOLDER_LS)
execute_process(COMMAND grep \\.hpp$ INPUT_FILE COMMAND_FOLDER_LS 
										OUTPUT_VARIABLE COMMAND_NAMES_HPP)
execute_process(COMMAND grep \\.cpp$ INPUT_FILE COMMAND_FOLDER_LS 
										OUTPUT_VARIABLE COMMAND_NAMES_CPP)

#Make the hpp names into a bunch of include statements
string(REPLACE "\n" "" INCLUDE_HPP "${COMMAND_NAMES_HPP}")
string(REGEX REPLACE ".hpp$" ".hpp\"\n};" INCLUDE_HPP "${INCLUDE_HPP}")
MESSAGE( "----------" )
MESSAGE("${INCLUDE_HPP}")
string(REPLACE ".hpp" ".hpp\"\n#include \"" INCLUDE_HPP "${INCLUDE_HPP}")
MESSAGE( "----------" )
MESSAGE("${INCLUDE_HPP}")


#Make the hpp file
file(WRITE "${OUTFILE}.hpp" "namespace ${OUTFILE} {\n\n")
MESSAGE( "----------" )
MESSAGE("${INCLUDE_HPP}")
file(APPEND "${OUTFILE}.hpp" "\n\n}")


#Remove newlines and duplicate the string
string(REPLACE "\n" "" COMMAND_NAMES "${COMMAND_NAMES_CPP}")
set(GEN_FNS "${COMMAND_NAMES}")

#Format the commands into an array of 
#function pointers to their 'generate' functions
string(REGEX REPLACE ".cpp$" "::generate\n};" GEN_FNS "${GEN_FNS}")
string(REPLACE ".cpp" "::generate,\n\t" GEN_FNS "${GEN_FNS}")

#Format the command names into an array
set(COMMAND_NAMES "\"${COMMAND_NAMES}")
string(REGEX REPLACE ".cpp$" "\"\n};" COMMAND_NAMES "${COMMAND_NAMES}")
string(REPLACE ".cpp" "\",\n\t\"" COMMAND_NAMES  "${COMMAND_NAMES}")

#Make the cpp file
file(WRITE "${OUTFILE}.cpp" "#include \"${OUTFILE}.hpp\"\n\n")
file(APPEND "${OUTFILE}.cpp" "//For ease of reading\n")
file(APPEND "${OUTFILE}.cpp" "using namespace ${OUTFILE};\n\n")
file(APPEND "${OUTFILE}.cpp" "//Match file index to Command generate function.\n")
file(APPEND "${OUTFILE}.cpp" "Command (*cmdsArr[]) (std::string s) = {\n\t")
file(APPEND "${OUTFILE}.cpp" "${GEN_FNS}\n\n")
file(APPEND "${OUTFILE}.cpp" "//Match file index to Command name.\n")
file(APPEND "${OUTFILE}.cpp" "char*[] cmdNames = {\n\t")
file(APPEND "${OUTFILE}.cpp" "${COMMAND_NAMES}\n")

#[[
#Find the commands .cpp's
file(GLOB commandCPPs RELATIVE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"Commands/*.cpp"
)

#Create a.out
add_executable(a.out
	Pi-File-Server/main.cpp
	Pi-File-Server/Server.cpp
	Pi-File-Server/FileHandler.cpp 
	${commandCPPs} )

#Link pthreads
target_link_libraries( a.out ${CMAKE_THREAD_LIBS_INIT} )
]]
