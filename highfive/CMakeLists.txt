cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(h5demos CXX)
set(CMAKE_C_FLAGS $ENV{CFLAGS})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories (SYSTEM 
                    $ENV{MPI_INC}
                    $ENV{HDF5_INC}
                    $ENV{HIGHFIVE_INC}
                    $ENV{ROOTSYS}/include
                    $ENV{CMSSW_BASE}/src)
link_directories ($ENV{ROOTSYS}/lib)
set (libraries ${libraries} 
               $ENV{MPI_LIB}/libmpi.so
               $ENV{HDF5_LIB}/libhdf5.so
               Core
               RIO
               Tree
               Hist)

add_executable(main main.cc)
target_link_libraries(main ${libraries})

add_executable(read_h5_deserialize read_h5_deserialize.cc)
target_link_libraries(read_h5_deserialize ${libraries})

