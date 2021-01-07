#-------------------------------------------------
#
# Project created by QtCreator 2017-12-20T12:57:57
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JetPrinterUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Lib/Build64/release/ -lJetPrinter -lOMC8000 -lSharedNodes
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Lib/Build64/debug/ -lJetPrinter -lOMC8000 -lSharedNodes

INCLUDEPATH += Lib/

DEPENDPATH += Lib/
DEPENDPATH += Lib/Build64/

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    jetprinterview.cpp \
    plcview.cpp \
    reportview.cpp \
    settingsview.cpp \
    programview.cpp \
    mainwindowinit.cpp \
    mainwindowsim.cpp \
    hexspinbox.cpp \
    spinboxdelegate.cpp \
    plcviewitem.cpp \
    plcviewmodel.cpp \
    loadview.cpp

HEADERS += \
    mainwindow.h \
    jetprinterview.h \
    plcview.h \
    reportview.h \
    settingsview.h \
    programview.h \
    lightwidget.h \
    tcpserver.h \
    udpserver.h \
    hexspinbox.h \
    spinboxdelegate.h \
    plcviewitem.h \
    plcviewmodel.h \
    loadview.h

FORMS += \
    mainwindow.ui \
    jetprinterview.ui \
    plcview.ui \
    reportview.ui \
    settingsview.ui \
    programview.ui \
    infodialog.ui \
    loadview.ui \
    warningdialog.ui

RESOURCES += \
    JetPrinterUI.qrc

RC_FILE += \
    Resource.rc
