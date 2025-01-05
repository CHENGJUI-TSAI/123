QT       += core gui network
RC_ICONS = Gobang.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QMAKE_CXXFLAGS += -fmax-include-depth=500

SOURCES += \
    Qtproject.cpp \
    aivsplayer.cpp \
    localgame.cpp \
    main.cpp \
    networkgame.cpp

HEADERS += \
    Qtproject.h \
    aivsplayer.h \
    localgame.h \
    networkgame.h

FORMS += \
    Qtproject.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
