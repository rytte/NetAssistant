QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += component

SOURCES += \
    component/AgentTest.cpp \
    component/Expand.cpp \
    component/NetServer.cpp \
    component/OpenAIClient.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    component/AgentTest.h \
    component/Expand.h \
    component/NetServer.h \
    component/OpenAIClient.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

RC_FILE += cover.rc
