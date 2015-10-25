#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <windows.h>
#include <QGLWidget>
#include <QMouseEvent>
#include <QTimerEvent>
#include "camera.h"
#include "leapmotion.h"
#include "btSoftBody.h"
#include "btQuickprof.h"
#include "btAlignedObjectArray.h"

#define CONTROL_LEAPMOTION 1
#define TIMER_START 1

class Lighting;
class btCollisionShape;
class SceneObjectDeformable;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btDynamicsWorld;
class btSoftRigidDynamicsWorld;
class btDefaultCollisionConfiguration;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void InitModel();
    void InitPhysics();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void timerEvent(QTimerEvent *);

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);

    ////////////////////////////////////////////////////////////
    /// \brief camera and light
    /// Public variables
    double cameraRadius;
    SphericalCamera *camera;

    //Light and mesh
    Lighting *lighting;

#if(CONTROL_LEAPMOTION)
    LeapMotion listener;
    Controller controller;
#endif

    int *secondaryDeformableObjectRenderingMesh_interpolation_vertices;
    double *secondaryDeformableObjectRenderingMesh_interpolation_weight;
    int secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices;
    SceneObjectDeformable *deformableObjRenderingMesh, *secondaryDeformableObjectRenderingMesh;

    //////////////////////////////////////////////////////////
    /// Bullet Physics
    ///
    //btClock m_clock;
    btCollisionObject *bt_Sphere;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
    btBroadphaseInterface* m_broadphase;
    btCollisionDispatcher* m_dispatcher;
    btConstraintSolver* m_solver;
    btDynamicsWorld* m_dynamicWorld;
    btSoftBodyWorldInfo m_softBodyWorldInfo;
    btDefaultCollisionConfiguration* m_collisionConfiguration;

    virtual btSoftRigidDynamicsWorld* getSoftDynamicWorld(){
        return (btSoftRigidDynamicsWorld*)m_dynamicWorld;
    }

    // mouse pose
    int g_vMousePos[2];
    // mouse button
    int g_leftButton, g_MidButton, g_rightButton;

signals:

public slots:

};

#endif // GLWIDGET_H
