cmake_minimum_required(VERSION 3.0)
project(Pi-File-Server)

#Require pthreads
find_package(Threads REQUIRED)


# --------------------Create Auto-generated files--------------------


#Auto generated file
set(OUTFILE "AutoGen")

#Find the hpp files in the Commands directory
execute_process(COMMAND ls ../Commands/ OUTPUT_FILE COMMAND_FOLDER_LS)
execute_process(COMMAND grep \\.hpp$ INPUT_FILE COMMAND_FOLDER_LS 
                                     OUTPUT_VARIABLE ALL_HPP_NAMES)

#Count the number of files
file(WRITE "build/COMMAND_FOLDER_LS_HPP" "${ALL_HPP_NAMES}")
execute_process(COMMAND wc -l INPUT_FILE COMMAND_FOLDER_LS_HPP
                              OUTPUT_VARIABLE NUM_FILES)
string(REPLACE "\n" "" NUM_FILES "${NUM_FILES}")
string(REPLACE " " "" NUM_FILES "${NUM_FILES}")

# ----------For the hpp----------

	#Make the hpp names into a bunch of include statements
	set(INC_STR "#include \"../Commands/")
	string(REPLACE ".hpp\n" ".hpp\"\n${INC_STR}" INCLUDE_HPP "${ALL_HPP_NAMES}")
	string(REGEX REPLACE "\n${INC_STR}$" "\n" INCLUDE_HPP "${INCLUDE_HPP}")
	set(INCLUDE_HPP "${INC_STR}${INCLUDE_HPP}")

	#Make the hpp file
	file(WRITE "build/${OUTFILE}.hpp" "#ifndef AUTOGEN_HPP\n")
	file(APPEND "build/${OUTFILE}.hpp" "#define AUTOGEN_HPP\n\n")
	file(APPEND "build/${OUTFILE}.hpp" "//System includes\n")
	file(APPEND "build/${OUTFILE}.hpp" "#include <string>\n\n")
	file(APPEND "build/${OUTFILE}.hpp" "//My includes\n")
	file(APPEND "build/${OUTFILE}.hpp" "#include \"../Pi-File-Server/AbstractCommand.hpp\"\n")
	file(APPEND "build/${OUTFILE}.hpp" "${INCLUDE_HPP}\n\n")
	file(APPEND "build/${OUTFILE}.hpp" "//To protect the global namespace\n")
	file(APPEND "build/${OUTFILE}.hpp" "namespace ${OUTFILE} {\n\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "//Match file index to Command generate function.\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "extern const Cmd * cmdsArr[${NUM_FILES}];\n\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "//Match file index to Command name.\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "extern const std::string cmdNames[${NUM_FILES}];\n\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "//The number of length of each array\n\t")
	file(APPEND "build/${OUTFILE}.hpp" "extern const int numCmds;\n\n")
	file(APPEND "build/${OUTFILE}.hpp" "}\n\n")
	file(APPEND "build/${OUTFILE}.hpp" "#endif")

# ----------For the cpp----------

	#Get the cpp files in two copies of a string without newlines.
	string(REPLACE ".hpp" ".cpp" COMMAND_NAMES "${ALL_HPP_NAMES}")
	string(REPLACE "\n" "" COMMAND_NAMES "${COMMAND_NAMES}")
	set(GEN_FNS "${COMMAND_NAMES}")

	#Format the commands into an array constructors 
	string(REGEX REPLACE ".cpp$" "()\n};" GEN_FNS "${GEN_FNS}")
	string(REPLACE ".cpp" "(),\n\tnew " GEN_FNS "${GEN_FNS}")

	#Format the command names into an array
	set(COMMAND_NAMES "\"${COMMAND_NAMES}")
	string(REGEX REPLACE ".cpp$" "\"\n};" COMMAND_NAMES "${COMMAND_NAMES}")
	string(REPLACE ".cpp" "\",\n\t\"" COMMAND_NAMES  "${COMMAND_NAMES}")

	#Make the cpp file
	file(WRITE "build/${OUTFILE}.cpp" "#include \"${OUTFILE}.hpp\"\n\n")
	file(APPEND "build/${OUTFILE}.cpp" "//Match file index to Command generate function.\n")
	file(APPEND "build/${OUTFILE}.cpp" "const Cmd * AutoGen::cmdsArr[${NUM_FILES}] = {\n\tnew ")
	file(APPEND "build/${OUTFILE}.cpp" "${GEN_FNS}\n\n")
	file(APPEND "build/${OUTFILE}.cpp" "//Match file index to Command name.\n")
	file(APPEND "build/${OUTFILE}.cpp" "const std::string AutoGen::cmdNames[${NUM_FILES}] = {\n\t")
	file(APPEND "build/${OUTFILE}.cpp" "${COMMAND_NAMES}\n\n")
	file(APPEND "build/${OUTFILE}.cpp" "//The number of length of each array\n")
	file(APPEND "build/${OUTFILE}.cpp" "const int AutoGen::numCmds = ${NUM_FILES};\n")


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
	build/${OUTFILE}.cpp
	Pi-File-Server/main.cpp
	Pi-File-Server/Server.cpp
	Pi-File-Server/FileHandler.cpp 
	Pi-File-Server/CommandHandler.cpp 
	Pi-File-Server/AbstractCommand.cpp 
	${commandCPPs}
)

#Link pthreads
target_link_libraries( a.out ${CMAKE_THREAD_LIBS_INIT} )

