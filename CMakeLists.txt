cmake_minimum_required(VERSION 3.14)

project(CallCenter_Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
add_definitions(-DBOOST_LOG_DYN_LINK=1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
set(SOURCES
    Sources/Configjson.cpp
    Sources/Serverworker.cpp
    Sources/Callprocessing.cpp
    Sources/Server.cpp
    Sources/CDRWorker.cpp
    Sources/Caller.cpp
    Sources/main.cpp

)

set(HEADERS
    Include/Configjson.h
    Include/Serverworker.h
    Include/Callprocessing.h
    Include/Server.h
    Include/CDRWorker.h
    Include/Caller.h
    Include/Mylogger.h
)

add_executable(CallCenter_Server ${SOURCES} ${HEADERS})

target_link_libraries(CallCenter_Server  Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(CallCenter_Server
    boost_log_setup
    boost_log
    boost_thread
    pthread
    boost_system
)
target_link_libraries(CallCenter_Server  -lstdc++)

