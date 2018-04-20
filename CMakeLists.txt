cmake_minimum_required(VERSION 2.8.3)
project(sagast)

set(CMAKE_VERBOSE_MAKEFILE "true")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g -fopenmp")

find_package(catkin REQUIRED COMPONENTS
    roscpp
    std_msgs
    sensor_msgs
    cv_bridge
    camera_model
    vignetting_model
    message_filters
    code_utils
    )

find_package(OpenCV REQUIRED)
find_package(Eigen3  REQUIRED)

catkin_package(
    INCLUDE_DIRS include
    )

include_directories("include")

include_directories(
    ${catkin_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    )

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(FRONTEND_LIB_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/feature/agast.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/score/agast_score.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/AGAST_8/AGAST_8.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/AGAST_8/OffsetsTable_8.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/AGAST_12d/AGAST_12d.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/AGAST_12d/OffsetsTable_12d.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/AGAST_12s/AGAST_12s.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/AGAST_12s/OffsetsTable_12s.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/OAST_16/OAST_16.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/agast_core/OAST_16/OffsetsTable_16.cpp
    ${PROJECT_SOURCE_DIR}/src/feature/offsets/getOffsets.cpp
)

add_executable(run_sagast
    src/sagast.cpp
    ${FRONTEND_LIB_SOURCE_FILES}
    )
target_link_libraries(run_sagast dw ${catkin_LIBRARIES} ${OpenCV_LIBS})

