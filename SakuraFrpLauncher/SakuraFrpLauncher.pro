# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
CONFIG(debug, debug|release) {
    TARGET = SakuraFrpLauncherd
    DESTDIR = ../../../../bin/Debug
    MOC_DIR += ./GeneratedFiles/Debug
    OBJECTS_DIR += Debug
}else{
    TARGET = SakuraFrpLauncher
    DESTDIR = ../../../../bin/Release
    MOC_DIR += ./GeneratedFiles/Release
    OBJECTS_DIR += Release
}

QT += core sql network widgets gui
CONFIG += debug_and_release
DEFINES += WIN64 QT_DLL QT_WIDGETS_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./../QT_Build/include \
    ./GeneratedFiles/Debug \
    ./../QT_Build/include/QtCore \
    ./../QT_Build/include/QtGui \
    ./../QT_Build/include/QtWidgets \
    ./../QT_Build/include/QtSql \
    ./../QT_Build/include/QtNetwork
LIBS += -L"./../QT_Build/lib"
DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(SakuraFrpLauncher.pri)
win32:RC_FILE = SakuraFrpLauncher.rc
