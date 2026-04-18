cmake_minimum_required(VERSION 3.15)
project(EmployeeParallelBenchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(TBB REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Threads REQUIRED)

add_executable(company 
    main.cpp
    Company.cpp
    Department.cpp
    Employee.cpp
    Manager.cpp
    Dialog.cpp
    io.cpp
    serialization_helpers.cpp
)

target_link_libraries(company PRIVATE 
    yaml-cpp 
    Threads::Threads
)

add_executable(speed_test 
    par_and_seq.cpp    
    Company.cpp
    Department.cpp
    Employee.cpp
    Manager.cpp
)

target_link_libraries(speed_test PRIVATE 
    yaml-cpp
    Threads::Threads
)