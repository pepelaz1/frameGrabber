#-------------------------------------------------
#
# Project created by QtCreator 2015-06-08T18:12:33
#
#-------------------------------------------------

QT       += core gui widgets winextras serialport
TARGET = FrameGrabberApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    listener.cpp

HEADERS  += mainwindow.h \
    listener.h

FORMS    += mainwindow.ui


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Shared/Logger-build-desktop-Qt_4_8_2__4_8_2__Release/release/ -lLogger
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Shared/Logger-build-desktop-Qt_4_8_2__4_8_2__Debug/debug/ -lLogger
#else:symbian: LIBS += -lLogger
#else:unix: LIBS += -L$$PWD/../../Shared/Logger-build-desktop-Qt_4_8_2__4_8_2__Debug/ -lLogger

#INCLUDEPATH += $$PWD/../../Shared/Logger
#DEPENDPATH += $$PWD/../../Shared/Logger

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Streams/build-Streams-Desktop_Qt_5_5_0_MSVC2010_32bit-Release/release/ -lStreams
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Streams/build-Streams-Desktop_Qt_5_5_0_MSVC2010_32bit-Debug/debug/ -lStreams


INCLUDEPATH += $$PWD/../../Streams/Streams
DEPENDPATH += $$PWD/../../Streams/Streams
