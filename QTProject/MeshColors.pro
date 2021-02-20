HEADERS += \
  colors.h \
  basic_object3d.h \
  object3d.h \
  axis.h \
  tetrahedron.h \
  glwidget.h \
  window.h \
  file_ply_stl.ch \
  object3d_ply.h

SOURCES += \
  basic_object3d.cc \
  object3d.cc \
  axis.cc \
  tetrahedron.cc \
  main.cc \
  glwidget.cc \
  window.cc \
  file_ply_stl.cc \
  object3d_ply.cpp

DISTFILES += \
  MeshColorsFragment.fsh \
  MeshColorsVertex.vsh

LIBS += -L/usr/X11R6/lib64 -lopengl32 -lglu32


CONFIG += c++11
QT += widgets gui
