project(dmplayer)
cmake_minimum_required(VERSION 3.0.5)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Qt4)

include_directories(${PROJECT_SOURCE_DIR} /usr/include/qt4)

#FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/basic/*.h ${PROJECT_SOURCE_DIR}/basic/*.cpp
#	${PROJECT_SOURCE_DIR}/h264/*.h ${PROJECT_SOURCE_DIR}/h264/*.cpp
#	${PROJECT_SOURCE_DIR}/hls/*.h ${PROJECT_SOURCE_DIR}/hls/*.cpp
#	${PROJECT_SOURCE_DIR}/http/*.h ${PROJECT_SOURCE_DIR}/http/*.cpp
#	${PROJECT_SOURCE_DIR}/mp2ts/*.h ${PROJECT_SOURCE_DIR}/mp2ts/*.cpp
#	${PROJECT_SOURCE_DIR}/mpa/*.h ${PROJECT_SOURCE_DIR}/mpa/*.cpp
#	${PROJECT_SOURCE_DIR}/rtp/*.h ${PROJECT_SOURCE_DIR}/rtp/*.cpp
#	${PROJECT_SOURCE_DIR}/rtsp/*.h ${PROJECT_SOURCE_DIR}/rtsp/*.cpp
#	${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/${MOC_SOURCES})
FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/*.cpp
                  ${PROJECT_SOURCE_DIR}/core/*.h ${PROJECT_SOURCE_DIR}/core/*.cpp
                  ${PROJECT_SOURCE_DIR}/rtsp/*.h ${PROJECT_SOURCE_DIR}/rtsp/*.cpp
                  ${PROJECT_SOURCE_DIR}/h264/*.h ${PROJECT_SOURCE_DIR}/h264/*.cpp)

add_executable(dmplayer  ${SOURCES} rtsp/dmmediasession.cpp rtsp/dmmediasession.h h264/dmmediasession.cpp h264/dmmediasession.h h264/dmdecoder.cpp h264/dmdecoder.h dmqplayer.cpp dmqplayer.h core/dmbaseplayer.cpp core/dmbaseplayer.h h264/dmbitreader.cpp h264/dmbitreader.h h264/dmsps.cpp h264/dmsps.h)
target_link_libraries(dmplayer QtCore QtGui avformat avcodec avutil avresample swscale ssl crypto pthread asound z X11)
set_target_properties(dmplayer PROPERTIES AUTOMOC true)
