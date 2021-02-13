HEADERS += \
  colors.h \
  basic_object3d.h \
  object3d.h \
  axis.h \
  object3d_meshcolors.h \
  object3d_ply.h \
  tetrahedron.h \
  glwidget.h \
  window.h \
  file_ply_stl.ch

SOURCES += \
  basic_object3d.cc \
  object3d.cc \
  axis.cc \
  object3d_meshcolors.cpp \
  object3d_ply.cpp \
  tetrahedron.cc \
  main.cc \
  glwidget.cc \
  window.cc \
  file_ply_stl.cc

DISTFILES += \
  MeshColorsFragment.fsh \
  MeshColorsVertex.vsh

LIBS += -L/usr/X11R6/lib64 -lopengl32 -lglu32


CONFIG += c++11
QT += widgets gui
