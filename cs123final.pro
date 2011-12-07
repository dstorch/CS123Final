QT += core \
    gui \
    opengl
TARGET = cs123final
TEMPLATE = app
INCLUDEPATH += scene \
    lib \
    math \
    support
DEPENDPATH += scene \
    lib \
    math \
    support
HEADERS += scene/glwidget.h \
    lib/targa.h \
    lib/glm.h \
    math/vector.h \
    support/resourceloader.h \
    support/mainwindow.h \
    support/camera.h \
    lib/targa.h \
    scene/grasscluster.h \
    math/matrix.h \
    math/matrix.inl \
    scene/grasspatch.h
SOURCES += scene/glwidget.cpp \
    lib/targa.cpp \
    lib/glm.cpp \
    support/resourceloader.cpp \
    support/mainwindow.cpp \
    support/main.cpp \
    support/camera.cpp \
    scene/grasscluster.cpp \
    scene/grasspatch.cpp
FORMS += mainwindow.ui \
    support/mainwindow.ui
OTHER_FILES += shaders/refract.vert \
    shaders/refract.frag \
    shaders/reflect.vert \
    shaders/reflect.frag \
    shaders/brightpass.frag \
    shaders/blur.frag
RESOURCES += 