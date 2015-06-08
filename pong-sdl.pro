TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    shader.cpp
QMAKE_CXXFLAGS += -std=c++14 -g -emit-llvm
include(deployment.pri)
qtcAddDeployment()

unix|win32: LIBS += -lGLEW -glfw -lGL -lX11 -lpthread -lXrandr -lXi
unix|win32: LIBS += -lSDL2 -lSDL2_mixer
DISTFILES += \
    shader.vert \
    framebuffer_shader.vert \
    shader.frag \
    assets/shaders/framebuffer_shader.frag \
    assets/shaders/shader.frag \
    assets/shaders/framebuffer_shader.vert \
    assets/shaders/shader.vert \
    assets/sounds/pong.wav \
    assets/sounds/pong2.wav \
    assets/sounds/pong3.wav

HEADERS += \
    shader.h \
    game.h

