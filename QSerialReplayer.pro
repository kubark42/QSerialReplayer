#-------------------------------------------------
#
# Project created by QtCreator 2014-04-11T22:56:02
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSerialReplayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    seriallogging.cpp \
    serialplayback.cpp \
    serialportconfigdialog.cpp

HEADERS  += mainwindow.h \
    seriallogging.h \
    serialplayback.h \
    serialportconfigdialog.h

FORMS    += mainwindow.ui \
    serialportconfigdialog.ui
