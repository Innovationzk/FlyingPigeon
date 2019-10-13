#-------------------------------------------------
#
# Project created by QtCreator 2019-10-09T10:20:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clientQTtest
TEMPLATE = app


SOURCES += source/friendlistitem.cpp \
    source/friendlistscreen.cpp \
    source/loginscreen.cpp \
    source/main.cpp \
    source/uicontroller.cpp \
    source/chatscreen.cpp

HEADERS  += include/clientapp/clientapp.h \
    include/clientapp/fileappdata.h \
    include/clientapp/fileappinstance.h \
    include/clientapp/msgappdata.h \
    include/clientapp/msgappinstance.h \
    include/friendlistitem.h \
    include/friendlistscreen.h \
    include/loginscreen.h \
    include/uicontroller.h \
    include/chatscreen.h

INCLUDEPATH += include/
INCLUDEPATH += common/
DEPENDPATH += common/


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/release/ -llib_json
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/debug/ -llib_json

INCLUDEPATH += $$PWD/../lib/Debug
DEPENDPATH += $$PWD/../lib/Debug

