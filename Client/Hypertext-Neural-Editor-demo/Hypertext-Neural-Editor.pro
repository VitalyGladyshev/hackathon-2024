QT       += core gui xml network
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aichat.cpp \
    documentwindow.cpp \
    filemanager.cpp \
    finddialog.cpp \
    helpviewdialog.cpp \
    hyperlinkdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    aichat.h \
    documentwindow.h \
    filemanager.h \
    finddialog.h \
    helpviewdialog.h \
    hyperlinkdialog.h \
    mainwindow.h \
    settings.h

TRANSLATIONS += \
    Hypertext-Neural-Editor_ru_RU.ts \
    Hypertext-Neural-Editor_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    teresources.qrc

FORMS += \
    aichat.ui \
    finddialog.ui \
    helpviewdialog.ui \
    hyperlinkdialog.ui
