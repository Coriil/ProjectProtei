cmake_minimum_required(VERSION 3.5)

project(cfg LANGUAGES CXX)

enable_testing()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Threads REQUIRED)



if ($ENV{GOOGLETEST_DIR})
    set(GOOGLETEST_DIR $ENV{GOOGLETEST_DIR})
else ()
    if (NOT "${CMAKE_CURRENT_SOURCE_DIR}/googletest/" STREQUAL "")
        message(WARNING "Using googletest src dir specified at Qt Creator wizard")
    endif ()
    set(GOOGLETEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
endif ()


if (EXISTS ${GOOGLETEST_DIR})
    set(GTestSrc ${GOOGLETEST_DIR}/googletest)
    set(GMockSrc ${GOOGLETEST_DIR}/googlemock)
elseif (UNIX AND EXISTS /usr/src/gtest)
    set(GTestSrc /usr/src/gtest)
    message(WARNING "Using gtest from system")
    if (EXISTS /usr/src/gmock)
        set(GMockSrc /usr/src/gmock)
    endif ()
else ()
    message( FATAL_ERROR "No googletest src dir found - set GOOGLETEST_DIR to enable!")
endif ()

set(GTestFiles ${GTestSrc}/src/gtest-all.cc)
set(GTestIncludes ${GTestSrc} ${GTestSrc}/include)
if (NOT ${GMockSrc} STREQUAL "")
    list(APPEND GTestFiles ${GMockSrc}/src/gmock-all.cc)
    list(APPEND GTestIncludes ${GMockSrc} ${GMockSrc}/include)
endif ()

include_directories(${GTestIncludes})
add_definitions(-DBOOST_LOG_DYN_LINK=1)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR}/Include)

set(SOURCES
    main.cpp
    Test_Configjson.cpp
    ${PARENT_DIR}/Sources/Configjson.cpp
     ${PARENT_DIR}/Sources/Serverworker.cpp
     ${PARENT_DIR}/Sources/Callprocessing.cpp
     ${PARENT_DIR}/Sources/Server.cpp
     ${PARENT_DIR}/Sources/CDRWorker.cpp
)

set(HEADERS
    ${PARENT_DIR}/Include/Configjson.h
    ${PARENT_DIR}/Include/Serverworker.h
    ${PARENT_DIR}/Include/Callprocessing.h
    ${PARENT_DIR}/Include/Server.h
    ${PARENT_DIR}/Include/CDRWorker.h
)

add_executable(cfg
               ${SOURCES}
               ${HEADERS}
               ${GTestFiles}
)


add_test(NAME cfg COMMAND cfg)

target_link_libraries(cfg  Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(cfg -lm Threads::Threads)
target_link_libraries(cfg -lm stdc++)
target_link_libraries(cfg
    boost_log_setup
    boost_log
    boost_thread
    pthread
    boost_system
    -lstdc++
)

