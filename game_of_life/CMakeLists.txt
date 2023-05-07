cmake_minimum_required(VERSION 3.11.0)
project(game_of_life)

find_package(MPI	REQUIRED)

set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
set(MPI_GUESS_LIBRARY_NAME MPICH2)

message(STATUS ${MPI_INCLUDE_PATH}) 
message(STATUS ${MPI_C_LIBRARIES}) 

set (EXEC_FILE "game_of_life")

set (SOURCES 
            ./srcs/main.cc
            ./srcs/game_of_life.cc
    )

add_executable(${EXEC_FILE} ${SOURCES})
target_include_directories(${EXEC_FILE} PRIVATE libs)

target_compile_features(${EXEC_FILE} PRIVATE cxx_std_20)

set (COMPILER_SETTINGS -O2)

target_compile_options(${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS})
