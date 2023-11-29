QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += BOOST_LOG_DYN_LINK="1"

LIBS += -lboost_log_setup -lboost_log -lboost_thread -lpthread -lboost_system
SOURCES += \
        CDRWorker.cpp \
        Callprocessing.cpp \
        Configjson.cpp \
        Server.cpp \
        Serverworker.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CDRWorker.h \
    Callprocessing.h \
    Configjson.h \
    Server.h \
    Serverworker.h


