greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = firmataTerm
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp \
    fdevice.cpp \
    util.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    console.h \
    fdevice.h \
    util.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    firmataterm.qrc

#HEADERS +=

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/firmatator/release/ -lfirmatator
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/include/firmatator/debug/ -lfirmatator
#else:symbian: LIBS += -lfirmatator
#else:unix: LIBS += -L$$PWD/include/firmatator/ -lfirmatator

#INCLUDEPATH += $$PWD/include/firmatator
#DEPENDPATH += $$PWD/include/firmatator

