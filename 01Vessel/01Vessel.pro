#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T20:25:37
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 01Vessel
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/build

LIBS +=  -L"$$PWD/../lib"  -L"D:/Program/LeapSDK/lib/x86"
LIBS += -lglu32 -lLeap -lVegalib  -lBullet

SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    leapmotion.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    leapmotion.h

FORMS    += mainwindow.ui

INCLUDEPATH += \
    D:/workspace/qt/QtBullet/01Vessel\
    ../_Bullet  \
    ../_Bullet/BulletCollision/BroadphaseCollision \
    ../_Bullet/BulletCollision/CollisionDispatch \
    ../_Bullet/BulletCollision/CollisionShapes \
    ../_Bullet/BulletCollision/Gimpact \
    ../_Bullet/BulletCollision/NarrowPhaseCollision  \
    ../_Bullet/BulletDynamics/Character \
    ../_Bullet/BulletDynamics/ConstraintSolver \
    ../_Bullet/BulletDynamics/Dynamics \
    ../_Bullet/BulletDynamics/Featherstone \
    ../_Bullet/BulletDynamics/MLCPSolvers \
    ../_Bullet/BulletDynamics/Vehicle \
    ../_Bullet/BulletSoftBody \
    ../_Bullet/LinearMath \
    ../_Vegalib/ \
    ../_Vegalib/objMesh \
    ../_Vegalib/sceneObject \
    ../_Vegalib/minivector \
    ../_Vegalib/imageIO \
    ../_Vegalib/graph \
    ../_Vegalib/volumetricMesh \

INCLUDEPATH += D:/Program/LeapSDK/include \

# DISABLE FEW WARNINGS
#QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-ignored-qualifiers

OTHER_FILES += \
    demo/demo1.jpg \
    demo/demo2.jpg
