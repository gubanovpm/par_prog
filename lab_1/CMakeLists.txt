cmake_minimum_required(VERSION 3.4.0)
project(par_laba)

find_package(MPI	REQUIRED)
find_package(Matplot++ REQUIRED)

set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
set(MPI_GUESS_LIBRARY_NAME MPICH2)

message(STATUS ${MPI_INCLUDE_PATH}) 
message(STATUS ${MPI_C_LIBRARIES}) 

set (EXEC_FILE "laba")

set (SOURCES 
            main.cc
            transfer_equation.cc
    )

add_executable(${EXEC_FILE} ${SOURCES})
target_include_directories(${EXEC_FILE} PRIVATE libs)

target_compile_features(${EXEC_FILE} PRIVATE cxx_std_20)

target_link_libraries(${EXEC_FILE} PRIVATE
                        Matplot++::matplot
                     )

set (COMPILER_SETTINGS
    -O2
)

target_compile_options(${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS})
