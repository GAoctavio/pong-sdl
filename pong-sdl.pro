TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    shader.cpp
QMAKE_CXXFLAGS += -std=c++14 -g
include(deployment.pri)
qtcAddDeployment()

unix|win32: LIBS += -lGLEW -glfw -lGL -lX11 -lpthread -lXrandr -lXi
unix|win32: LIBS += -lSDL2

DISTFILES += \
    shader.vert \
    framebuffer_shader.frag \
    framebuffer_shader.vert \
    shader.frag

HEADERS += \
    shader.h \
    game.h
