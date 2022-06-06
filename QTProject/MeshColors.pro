TARGET = MeshColors

TEMPLATE = app

INCLUDEPATH += $$PWD/src

DEFINES += PROJECT_PATH=\"\\\"$${_PRO_FILE_PWD_}/\\\"\"

HEADERS += \
    $$PWD/src/basic_object3d.h \
    $$PWD/src/meshcolorsface.h \
    $$PWD/src/meshcolorsobject3d.h \
    $$PWD/src/movableobject.h \
    $$PWD/src/object3d.h \
    $$PWD/src/axis.h \
    $$PWD/src/glwidget.h \
    $$PWD/src/window.h \
    $$PWD/src/loadingwindow.h \
    $$PWD/src/file_ply_stl.ch

SOURCES += \
    $$PWD/src/basic_object3d.cc \
    $$PWD/src/meshcolorsface.cpp \
    $$PWD/src/meshcolorsobject3d.cpp \
    $$PWD/src/movableobject.cpp \
    $$PWD/src/object3d.cc \
    $$PWD/src/axis.cc \
    $$PWD/src/main.cc \
    $$PWD/src/glwidget.cc \
    $$PWD/src/window.cc \
    $$PWD/src/loadingwindow.cpp \
    $$PWD/src/file_ply_stl.cc

DISTFILES += \
    $$PWD/src/shaders/MeshColorsFragment.fsh \
    $$PWD/src/shaders/MeshColorsVertex.vsh \
    $$PWD/src/shaders/MeshColorsGeometry.gsh \
    $$PWD/src/shaders/BaseFragment.fsh \
    $$PWD/src/shaders/BaseVertex.vsh

win32 {
    DEFINES += PLATFORM_WINDOWS=1
    LIBS += -L/usr/X11R6/lib64 -lopengl32 -lglu32
}

CONFIG += c++11 --stack 40000
QT += opengl widgets gui
