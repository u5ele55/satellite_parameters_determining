cmake_minimum_required (VERSION 3.10)
project(satellite_parameters_determining LANGUAGES CXX)

file(GLOB UTILS utils/*/*/*.cpp utils/*/*.cpp utils/*.cpp)
file(GLOB GLBL global/*.cpp)
file(GLOB CORE core/*.cpp core/*/*.cpp)
file(GLOB RADIOTELESCOPES radiotelescopes/*.cpp)
file(GLOB LEAST_SQUARES least_squares/*.cpp least_squares/*/*.cpp)
file(GLOB INTEGRATION integration/*/*.cpp)

add_subdirectory(utils)

add_executable(run 
    main.cpp
    ${UTILS}
    ${GLBL}
    ${CORE}
    ${RADIOTELESCOPES}
    ${LEAST_SQUARES}
    ${INTEGRATION}
    )

target_include_directories(
    run 
    PUBLIC 
    utils
    ${CMAKE_CURRENT_SOURCE_DIR}
)