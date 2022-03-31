HEADERS += \
  colors.h \
  basic_object3d.h \
  meshcolorsface.h \
  meshcolorsobject3d.h \
  movableobject.h \
  object3d.h \
  axis.h \
  glwidget.h \
  window.h \
  file_ply_stl.ch

SOURCES += \
  basic_object3d.cc \
  meshcolorsface.cpp \
  meshcolorsobject3d.cpp \
  movableobject.cpp \
  object3d.cc \
  axis.cc \
  main.cc \
  glwidget.cc \
  window.cc \
  file_ply_stl.cc

DISTFILES += \
  MeshColorsFragment.fsh \
  MeshColorsVertex.vsh \
  MeshColorsGeometry.gsh \
  BaseFragment.fsh \
  BaseVertex.vsh

win32 {
    DEFINES += PLATFORM_WINDOWS=1
    LIBS += -L/usr/X11R6/lib64 -lopengl32 -lglu32
}

CONFIG += c++11 --stack 40000
QT += opengl widgets gui
