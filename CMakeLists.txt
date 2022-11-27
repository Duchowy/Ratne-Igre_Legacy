cmake_minimum_required(VERSION 3.1.0)
project (ratne)
include(CTest)
set(CMAKE_VERBOSE_MAKEFILE ON)
enable_testing()

# path variables

set(LIB_DIR ${PROJECT_SOURCE_DIR}/deps_sfml/lib)
set(INCL_DIR ${PROJECT_SOURCE_DIR}/deps_sfml/include)

# prevent for in-source building
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
message(SEND_ERROR "In-source builds are not allowed.")
endif ()

add_definitions(-DPROJECTPATH=\"${PROJECT_SOURCE_DIR}\")

set(CMAKE_BUILD_TYPE Debug) #Release
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O2 -static-libgcc -static-libstdc++")

if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif(WIN32)

# dependency area
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5.1 COMPONENTS audio graphics window system REQUIRED PATHS ${PROJECT_SOURCE_DIR}/deps_sfml)

#find_package(Freetype PATHS ${PROJECT_SOURCE_DIR}/deps_sfml)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()




if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    message("Debug build.")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    message("Release build.")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)
endif()




set(HEADERS ${PROJECT_SOURCE_DIR}/include)
include_directories(${HEADERS})

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

#-lfreetype -lopengl32 -lwinmm -lgdi32
target_link_libraries(${PROJECT_NAME} -fopenmp -lconfig++ -lpthread -lstdc++ sfml-audio sfml-graphics sfml-window sfml-system -lm)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCL_DIR} )

add_test(NAME launch COMMAND ./${PROJECT_NAME})
# add_test(NAME leak COMMAND valgrind --leak-check=full --track-origins=yes ./${PROJECT_NAME})