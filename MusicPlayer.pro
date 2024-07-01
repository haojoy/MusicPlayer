QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia sql

CONFIG += c++17


#程序版本
VERSION  = 1.0
#程序图标
RC_ICONS = images/logo.ico
#产品名称
QMAKE_TARGET_PRODUCT = MusicPlayer
#版权所有
QMAKE_TARGET_COPYRIGHT = haojoy
#文件说明
QMAKE_TARGET_DESCRIPTION = QT MusicPlayer

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwidget.cpp \
    myLyricWidget.cpp \
    myTableWidget.cpp \
    switchanimation.cpp

HEADERS += \
    mainwidget.h \
    music.h \
    myLyricWidget.h \
    mySlider.h \
    myTableWidget.h \
    switchanimation.h

FORMS += \
    mainwidget.ui

TRANSLATIONS += \
    MusicPlayer_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
