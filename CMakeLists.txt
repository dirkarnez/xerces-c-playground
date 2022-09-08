cmake_minimum_required(VERSION 3.23)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(xerces-c-playground)

find_package(XercesC 4.0.0 EXACT REQUIRED PATHS "${XercesC_DIR}")
find_package(Threads)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_executable(${PROJECT_NAME}
    "src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} XercesC::XercesC Threads::Threads)