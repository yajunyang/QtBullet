#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T21:01:27
#
#-------------------------------------------------

QT       -= core gui

TARGET = Vegalib
TEMPLATE = lib
CONFIG += staticlib
#DEFINES += _VEGALIB_LIBRARY

LIBS += -lopengl32 -lglu32

# 相对路径
DESTDIR=$$PWD/../lib

SOURCES += \
    objMesh/objMesh-disjointSet.cpp \
    objMesh/objMesh.cpp \
    objMesh/objMeshEncode.cpp \
    objMesh/objMeshRender.cpp \
    sceneObject/sceneObject.cpp \
    sceneObject/sceneObjectDeformable.cpp \
    sceneObject/sceneObjectWithRestPosition.cpp \
    minivector/eig3.cpp \
    minivector/mat3d.cpp \
    minivector/vec2d.cpp \
    minivector/vec3d.cpp \
    imageIO/imageIO.cpp \
    graph/camera.cpp \
    graph/lighting.cpp \
    volumetricMesh/tetMesh.cpp \
    volumetricMesh/volumetricMesh.cpp \
    volumetricMesh/volumetricMeshENuMaterial.cpp \
    volumetricMesh/volumetricMeshLoader.cpp \
    volumetricMesh/volumetricMeshMooneyRivlinMaterial.cpp \
    volumetricMesh/volumetricMeshOrthotropicMaterial.cpp \
    volumetricMesh/volumetricMeshParser.cpp

HEADERS += \
    objMesh/objMesh-disjointSet.h \
    objMesh/objMesh.h \
    objMesh/objMeshEncode.h \
    objMesh/objMeshRender.h \
    sceneObject/sceneObject.h \
    sceneObject/sceneObjectDeformable.h \
    sceneObject/sceneObjectWithRestPosition.h \
    macros.h \
    minivector/eig3.h \
    minivector/mat3d.h \
    minivector/minivector.h \
    minivector/vec2d.h \
    minivector/vec3d.h \
    imageIO/imageFormats.h \
    imageIO/imageIO.h \
    graph/camera.h \
    graph/lighting.h \
    vegalong.h \
    volumetricMesh/tetMesh.h \
    volumetricMesh/volumetricMesh.h \
    volumetricMesh/volumetricMeshENuMaterial.h \
    volumetricMesh/volumetricMeshLoader.h \
    volumetricMesh/volumetricMeshMooneyRivlinMaterial.h \
    volumetricMesh/volumetricMeshOrthotropicMaterial.h \
    volumetricMesh/volumetricMeshParser.h

INCLUDEPATH += \
    D:/workspace/qt/QtBullet/_Vegalib \
    graph  \
    imageIO  \
    minivector  \
    objMesh \
    sceneObject \
    volumetricMesh

# DISABLE FEW WARNINGS
#QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-ignored-qualifiers

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
