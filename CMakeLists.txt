project(dmplayer)
cmake_minimum_required(VERSION 3.0.5)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Qt4)

include_directories(${PROJECT_SOURCE_DIR} /usr/include/qt4)

FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.cpp
                  ${PROJECT_SOURCE_DIR}/core/*.h ${PROJECT_SOURCE_DIR}/core/*.cpp
                  ${PROJECT_SOURCE_DIR}/rtsp/*.h ${PROJECT_SOURCE_DIR}/rtsp/*.cpp
                  ${PROJECT_SOURCE_DIR}/h264/*.h ${PROJECT_SOURCE_DIR}/h264/*.cpp
                  ${PROJECT_SOURCE_DIR}/h264/parameters/*.h ${PROJECT_SOURCE_DIR}/h264/parameters/*.cpp
                  ${PROJECT_SOURCE_DIR}/h264/slice/*.h ${PROJECT_SOURCE_DIR}/h264/slice/*.cpp
                  ${PROJECT_SOURCE_DIR}/avi/*.h ${PROJECT_SOURCE_DIR}/avi/*.cpp
                  ${PROJECT_SOURCE_DIR}/aac/*.h ${PROJECT_SOURCE_DIR}/aac/*.cpp)

add_executable(dmplayer  ${SOURCES})
target_link_libraries(dmplayer QtCore QtGui avformat avcodec avutil avresample swscale ssl crypto pthread asound z X11)
set_target_properties(dmplayer PROPERTIES AUTOMOC true)
