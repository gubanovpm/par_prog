cmake_minimum_required(VERSION 3.4.0)
project(par_laba)

find_package(Matplot++ REQUIRED)

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