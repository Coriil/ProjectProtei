cmake_minimum_required(VERSION 3.14)

project(Json_Timer_Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
add_definitions(-DBOOST_LOG_DYN_LINK=1)

set(SOURCES
    Configjson.cpp
    Serverworker.cpp
    Callprocessing.cpp
    Server.cpp
    CDRWorker.cpp
    main.cpp
)

set(HEADERS
    Configjson.h
    Serverworker.h
    Callprocessing.h
    Server.h
    CDRWorker.h
)

add_executable(Json_Timer_Server ${SOURCES} ${HEADERS})

target_link_libraries(Json_Timer_Server  Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Json_Timer_Server
    boost_log_setup
    boost_log
    boost_thread
    pthread
    boost_system
)
target_link_libraries(Json_Timer_Server  -lstdc++)
