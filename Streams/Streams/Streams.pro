#QT       -= gui

QT        += widgets

TARGET = Streams
TEMPLATE = lib

DEFINES += STREAMS_LIBRARY

SOURCES += \
    playback_graph.cpp \
    graphfactory.cpp \
    vumeter.cpp \
    capture_graph.cpp \
    base_graph.cpp \
    videowindow.cpp \
    nplayback_graph.cpp \
    nrecorder_graph.cpp \
    textoverlay.cpp \
    overlayitem.cpp \
    overlayfilter.cpp \
    graphmanager.cpp \
    encmanager.cpp \
    srcmanager.cpp \
    rndmanager.cpp \
    informer_graph.cpp \
    proppagemanager.cpp

HEADERS += \
    Streams_global.h \
    igraph.h \
    playback_graph.h \
    graphfactory.h \
    qedit.h \
    vumeter.h \
    capture_graph.h \
    base_graph.h \
    videowindow.h \
    moreuuids.h \
    nplayback_graph.h \
    irecordergraph.h \
    nrecorder_graph.h \
    textoverlay.h \
    overlayitem.h \
    overlayfilter.h \
    graphmanager.h \
    encmanager.h \
    srcmanager.h \
    rndmanager.h \
    informer_graph.h \
    proppagemanager.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE514AECD
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Streams.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L"C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib/"  -lstrmiids
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib/" -lstrmiids

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Logger-build-desktop-Qt_4_8_2__4_8_2__Release/release/ -lLogger
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Logger-build-desktop-Qt_4_8_2__4_8_2__Debug/debug/ -lLogger
#else:symbian: LIBS += -lLogger
#else:unix: LIBS += -L$$PWD/../Logger-build-desktop-Qt_4_8_2__4_8_2__Release/ -lLogger

#INCLUDEPATH += $$PWD/../Logger
#DEPENDPATH += $$PWD/../Logger

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../DirectShow/Release/  -lstrmbase
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../DirectShow/Debug/  -lstrmbasd


INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Windows/v7.1/Samples/multimedia/directshow/baseclasses/"
LIBS += gdi32.lib
LIBS += winmm.lib
LIBS += user32.lib


INCLUDEPATH += "C:/WinDDK/7600.16385.1/inc/atl71/"

