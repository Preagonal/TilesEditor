# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = TilesEditor
DESTDIR = ./x64/Release
CONFIG += release
DEFINES += KGL_STATIC
INCLUDEPATH += ./ \
    ./QCodeEditor/include/ \
    ./QCodeEditor/include/KGL/Widgets/
LIBS += -L"."
DEPENDPATH += .
MOC_DIR += .
OBJECTS_DIR += release
UI_DIR += .
RCC_DIR += .
include(TilesEditor.pri)
