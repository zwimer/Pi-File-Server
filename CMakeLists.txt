cmake_minimum_required(VERSION 3.0)
project(Pi-File-Server)

#Require pthreads
find_package(Threads REQUIRED)


# --------------------Create Auto-generated files--------------------


#Auto generated file
set(OUTFILE "AutoGen")

#ls the commands directory
execute_process(COMMAND ls ../Pi-File-Server/ OUTPUT_FILE COMMAND_FOLDER_LS)

# ----------For the hpp----------

	#Find the hpp files
	execute_process(COMMAND grep \\.hpp$ INPUT_FILE COMMAND_FOLDER_LS 
											OUTPUT_VARIABLE INCLUDE_HPP)

	#Make the hpp names into a bunch of include statements
	string(REPLACE ".hpp\n" ".hpp\"\n#include \"" INCLUDE_HPP "${INCLUDE_HPP}")
	string(REGEX REPLACE "\n#include \"$" "\n" INCLUDE_HPP "${INCLUDE_HPP}")
	set(INCLUDE_HPP "#include \"${INCLUDE_HPP}")


	#Make the hpp file
	file(WRITE "build/${OUTFILE}.hpp" "${INCLUDE_HPP}\n\n")
	file(APPEND "build/${OUTFILE}.hpp" "//To protect the global namespace\n")
	file(APPEND "build/${OUTFILE}.hpp" "namespace ${OUTFILE} {\n\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "//Match file index to Command generate function.\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "Command (*cmdsArr[]) (std::string s);\n\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "//Match file index to Command name.\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "char*[] cmdNames;\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "\n}")

# ----------For the cpp----------

	#Find the cpp files
	execute_process(COMMAND grep \\.cpp$ INPUT_FILE COMMAND_FOLDER_LS 
											OUTPUT_VARIABLE COMMAND_NAMES)

	#Remove newlines and duplicate the string
	string(REPLACE "\n" "" COMMAND_NAMES "${COMMAND_NAMES}")
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
	file(WRITE "build/${OUTFILE}.cpp" "#include \"${OUTFILE}.hpp\"\n\n")
	file(APPEND "build/${OUTFILE}.cpp" "//For ease of reading\n")
	file(APPEND "build/${OUTFILE}.cpp" "using namespace ${OUTFILE};\n\n")
	file(APPEND "build/${OUTFILE}.cpp" "//Match file index to Command generate function.\n")
	file(APPEND "build/${OUTFILE}.cpp" "Command (*cmdsArr[]) (std::string s) = {\n\t")
	file(APPEND "build/${OUTFILE}.cpp" "${GEN_FNS}\n\n")
	file(APPEND "build/${OUTFILE}.cpp" "//Match file index to Command name.\n")
	file(APPEND "build/${OUTFILE}.cpp" "char*[] cmdNames = {\n\t")
	file(APPEND "build/${OUTFILE}.cpp" "${COMMAND_NAMES}\n")


# -----------------------Create the executable-----------------------


#Compile with C++11, -Wall, and -g
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

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

