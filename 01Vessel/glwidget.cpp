#include "glwidget.h"
#include <Windows.h>
#include <GL/glu.h>
#include "lighting.h"
#include "volumetricMesh.h"
#include "sceneObjectDeformable.h"
#include "btBulletDynamicsCommon.h"
#include "btSoftBodyHelpers.h"
#include "btDynamicsWorld.h"
#include "btSoftRigidDynamicsWorld.h"
#include "btSoftBodyRigidBodyCollisionConfiguration.h"

// Leap Motion
extern int leapIndex;
extern double leapRadius;
extern Leap::Vector handPosition;

const int maxProxies = 32766;
const int maxOverlap = 65535;

// dynamic world gravity
btVector3 d_gravity(0.0, -0.0, 0.0);

// vertices displacement and rest position
double *d_displacement, *d_initposition,*uSecondary;
/***********************************************************************************
 * VolumeMesh configuration
 * VegaFEM 1-index, Bullet 0-index
*/
struct Turtle{
#include "turtle.inl"
};

const char* secondaryRenderingMeshFilename[] = {"../models/turtle/turtle.obj"};
const char* renderingMeshFilename[] = {"../models/turtle/turtle-volumetric.obj"};
const char* secondaryRenderingMeshInterpolationFilename[] = {"../models/turtle/turtle.interp"};

// Fixed Vertices
int fixednodes[] = {
    0,2,5,7,10,11,12,14,16,17,25,28,41,44,46,
    48,57,58,59,246,247,255,264
};

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    cameraRadius = 12.6;
    m_dynamicWorld = NULL;

}

GLWidget::~GLWidget()
{
    qDebug("Deconstructor");
    free(d_displacement);
    free(uSecondary);
    free(d_initposition);

#if(CONTROL_LEAPMOTION)
    controller.removeListener(listener);
#endif

    // remove the rigidbodies from the dynamic world and delete them
    for(int i=m_dynamicWorld->getNumCollisionObjects()-1;i>=0;i--){
        btCollisionObject* obj = m_dynamicWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if(body && body->getMotionState()){
            delete body->getMotionState();
        }
        m_dynamicWorld->removeCollisionObject(obj);
        delete obj;
    }

    // delete collision shapes
    for(int j=0; j<m_collisionShapes.size(); j++){
        btCollisionShape * shape = m_collisionShapes[j];
        m_collisionShapes[j] = 0;
        delete shape;
    }

    delete m_dynamicWorld;  // delete dynamic world
    delete m_solver;        // delete solver
    delete m_broadphase;    // delete broadphase
    delete m_dispatcher;    // delete dispatcher
    delete m_collisionConfiguration;    // delete configuration
}

void GLWidget::InitModel()
{
    #define m_ModelIndex 0
    ///////////////////////////////////////////////////////////////////////////////
    // deformableObjRenderingMesh
    deformableObjRenderingMesh = new SceneObjectDeformable(renderingMeshFilename[m_ModelIndex]);
    //if(deformableTexture)
    deformableObjRenderingMesh->ResetDeformationToRest();
    deformableObjRenderingMesh->BuildNeighboringStructure();
    deformableObjRenderingMesh->BuildNormals();
    deformableObjRenderingMesh->SetMaterialAlpha(0.5);



    // get the rest position of the (Tetmesh)model
    d_initposition = (double *)calloc(3*deformableObjRenderingMesh->GetNumVertices(),sizeof(double));
    deformableObjRenderingMesh->GetVertexRestPositions(d_initposition);

    ///////////////////////////////////////////////////////////////////////////////
    // secondaryDeformableObjectRenderingMesh
    secondaryDeformableObjectRenderingMesh = new SceneObjectDeformable(secondaryRenderingMeshFilename[m_ModelIndex]);
    //if(RenderenableTexturees)
    secondaryDeformableObjectRenderingMesh->ResetDeformationToRest();
    secondaryDeformableObjectRenderingMesh->BuildNeighboringStructure();
    secondaryDeformableObjectRenderingMesh->BuildNormals();

    uSecondary = (double*)calloc(3 * secondaryDeformableObjectRenderingMesh->Getn(), sizeof(double));

    // Interpolation
    secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices =
            VolumetricMesh::getNumInterpolationElementVertices(secondaryRenderingMeshInterpolationFilename[m_ModelIndex]);
    if(secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices < 0){
        printf("Error: unable to open the file %s.\n",secondaryRenderingMeshInterpolationFilename[m_ModelIndex]);
        exit(0);
    }

    VolumetricMesh::loadInterpolationWeights(secondaryRenderingMeshInterpolationFilename[m_ModelIndex],secondaryDeformableObjectRenderingMesh->Getn(),
                                             secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices,&secondaryDeformableObjectRenderingMesh_interpolation_vertices,&secondaryDeformableObjectRenderingMesh_interpolation_weight);
}

btSoftBody* psb = NULL;
void GLWidget::InitPhysics()
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Bulllet Dynamic init
    ///
    // CollisionShape[0] == BOX
    btCollisionShape *groundBox = new btBoxShape(btVector3(100,1.5,100));
    m_collisionShapes.push_back(groundBox);

    // CollisionShape[0] == BOX
    btCollisionShape *t_sphere = new btSphereShape(leapRadius);
    m_collisionShapes.push_back(t_sphere);

    m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_softBodyWorldInfo.m_dispatcher = m_dispatcher;

    btVector3 worldAabbMin(-1000,-1000,-1000);
    btVector3 worldAabbMax( 1000, 1000, 1000);

    m_broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);
    m_softBodyWorldInfo.m_broadphase = m_broadphase;

    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
    m_solver = solver;

    // new SoftRigidDynamicWorld
    btDiscreteDynamicsWorld *world = new btSoftRigidDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
    m_dynamicWorld = world;
    m_dynamicWorld->setGravity(d_gravity);
    m_dynamicWorld->getDispatchInfo().m_enableSPU = true;

    //create ground object
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(0,-8.2f,0));

    btCollisionObject *newOb = new btCollisionObject();
    newOb->setWorldTransform(tr);
    newOb->setInterpolationWorldTransform( tr);

    //[0] is planar
    newOb->setCollisionShape(m_collisionShapes[0]);

    m_dynamicWorld->addCollisionObject(newOb);

    // LeapMotion physics
    bt_Sphere = new btCollisionObject();
    bt_Sphere->setCollisionShape(m_collisionShapes[1]);
    m_dynamicWorld->addCollisionObject(bt_Sphere);

    m_softBodyWorldInfo.m_gravity = d_gravity;
    m_softBodyWorldInfo.m_sparsesdf.Initialize();

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Client reset sence
    ///
    m_softBodyWorldInfo.m_sparsesdf.Reset();
    m_softBodyWorldInfo.air_density    = (btScalar)1.2;
    m_softBodyWorldInfo.water_density  = 0.0;
    m_softBodyWorldInfo.water_offset   = 0;
    m_softBodyWorldInfo.water_normal   = btVector3(0,0,0);
    m_softBodyWorldInfo.m_gravity      = d_gravity;

    /// \SoftBody
    psb = btSoftBodyHelpers::CreateFromTetGenData(m_softBodyWorldInfo,
                                                  Turtle::getElements(),
                                                  0,
                                                  Turtle::getNodes(),
                                                  false,true,true);
    btSoftBody::tNodeArray &nodes(psb->m_nodes);
    d_displacement = (double *)calloc(3*nodes.size(),sizeof(double));

    // fixed nodes
    for(unsigned int i=0;i<sizeof(fixednodes)/sizeof(int);i++){
            psb->setMass(fixednodes[i],0);
        }

    getSoftDynamicWorld()->addSoftBody(psb);
    psb->m_cfg.piterations = 2;

    psb->setTotalMass(100);
    psb->generateClusters(0);
    psb->getCollisionShape()->setMargin(0.01);
    psb->m_cfg.collisions = btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;

    psb->m_materials[0]->m_kLST = 1;
    psb->m_materials[0]->m_kAST = 1;
    psb->m_materials[0]->m_kVST = 1;
}

void GLWidget::initializeGL()
{
    glClearColor(0.0f,0.5f,0.5f,1.0f);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    InitModel();
    InitPhysics();

#if(CONTROL_LEAPMOTION)
    // connect leapmotion
    int iter = 0;
    while (iter < 100 ) {
        if(controller.isConnected()){
           controller.addListener(listener);
            break;
        }
        iter++;
    }
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Setup Lighting and Camera
    /// !1
    lighting = new Lighting();
    lighting->ModifyLight(3, -10.0f, 10.0f, -10.0f);
    lighting->ModifyLight(6,  10.0f, 10.0f,  20.0f);

    // light 0 and 4 for Leapmotion
    GLfloat sphere_l[]={0.8f,0.8f,0.8f,1.0f,
                        1.0f,1.0f,0.0f,1.0f};
    lighting->ModifyLight(0,  0.0f, 5.0f, 10.0f,1.0f,0.1f);
    memcpy(lighting->GetLightPtr(0)->lKd,&sphere_l[0], 4*sizeof(float));
    memcpy(lighting->GetLightPtr(0)->lKs,&sphere_l[0], 4*sizeof(float));

    lighting->ModifyLight(4,  0.0f, 5.0f, 10.0f,1.0f,0.1f);
    memcpy(lighting->GetLightPtr(4)->lKd,&sphere_l[4], 4*sizeof(float));
    memcpy(lighting->GetLightPtr(4)->lKs,&sphere_l[4], 4*sizeof(float));

    # define M_PI		3.14159265358979323846	/* pi */
    double cameraLongitude = -103.0, cameraLattitude = 8.0;
    double upPos[3] = {0,1,0}, focusPos[3] = {0.0,3.0,0.0};
    camera = new SphericalCamera(cameraRadius, 1.0 * cameraLongitude / 360 *(2*M_PI), 1.0 * cameraLattitude / 360*(2*M_PI),
                                 focusPos,upPos,0.05,1.0);

#if(TIMER_START)
    startTimer(30);
#endif
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);

    // frustrum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // ensure the display ratio
    gluPerspective(45.0,(double)w/h, cameraRadius*0.01, cameraRadius*100);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if(camera)
        camera->Look();

    lighting->GetLightPtr(0)->b_Enabled = false;
    lighting->GetLightPtr(4)->b_Enabled = false;
    lighting->GetLightPtr(3)->b_Enabled = true;
    lighting->GetLightPtr(6)->b_Enabled = true;
    lighting->LightScene();
    secondaryDeformableObjectRenderingMesh->Render();

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);
    glDrawBuffer(GL_NONE);
    if(deformableObjRenderingMesh)
        deformableObjRenderingMesh->Render();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDrawBuffer(GL_BACK);
    if(deformableObjRenderingMesh)
        deformableObjRenderingMesh->Render();
    glDisable(GL_BLEND);

    glColor3f(0.8f,0.8f,0.8f);
    deformableObjRenderingMesh->RenderEdges();


#if(CONTROL_LEAPMOTION)
    // Render LeapMotion position
    if(controller.isConnected()){
        if(leapIndex == 0)
            lighting->GetLightPtr(0)->b_Enabled = true;
        else
            lighting->GetLightPtr(4)->b_Enabled = true;
        lighting->GetLightPtr(3)->b_Enabled = false;
        lighting->GetLightPtr(6)->b_Enabled = false;

        lighting->LightScene();
        listener.RendLeap();
    }
#endif

    /// NEED LIGHTING ABOVE
    //////////////////////////////////////////////////////////////////////////////////

    // xyz-Axis
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINES);

    for(int i=0; i<3; i++){
        float color[3] = {0,0,0},vertex[3] = {0,0,0};;
        color[i] = 1;vertex[i] = 20;
        glColor3fv(color);
        glVertex3fv(vertex); glVertex3f(0,0,0);
    }
    glEnd();
    glLineWidth(1);

    glColor3f(1,0,0);
    glPointSize(6);
    btSoftBody::tNodeArray &nodes(psb->m_nodes);
    glBegin(GL_POINTS);

    for(unsigned int i=0; i<(sizeof(fixednodes)/sizeof(int));i++)
        glVertex3f(nodes[fixednodes[i]].m_x.x(),nodes[fixednodes[i]].m_x.y(),nodes[fixednodes[i]].m_x.z());
    glEnd();
    glPointSize(1);

}

void GLWidget::timerEvent(QTimerEvent *)
{
    if(leapIndex == 4){
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(handPosition.x,handPosition.y,handPosition.z));
        bt_Sphere->setWorldTransform(tr);
    }
//    bt_Sphere->setInterpolationWorldTransform( tr);

    // Fixed timestep
    if(m_dynamicWorld)
        m_dynamicWorld->stepSimulation(1.0/60);

    btSoftBody::tNodeArray &nodes(psb->m_nodes);
    for(int i=0; i<nodes.size(); i++){
        d_displacement[3*i + 0] = nodes[i].m_x.x() - d_initposition[3*i + 0];
        d_displacement[3*i + 1] = nodes[i].m_x.y() - d_initposition[3*i + 1];
        d_displacement[3*i + 2] = nodes[i].m_x.z() - d_initposition[3*i + 2];
    }

    deformableObjRenderingMesh->SetVertexDeformations(d_displacement);

    if(secondaryDeformableObjectRenderingMesh != NULL){
        VolumetricMesh::interpolate(d_displacement, uSecondary, secondaryDeformableObjectRenderingMesh->Getn(), secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices, secondaryDeformableObjectRenderingMesh_interpolation_vertices, secondaryDeformableObjectRenderingMesh_interpolation_weight);
        secondaryDeformableObjectRenderingMesh->SetVertexDeformations(uSecondary);
    }
    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    g_vMousePos[0] = e->pos().x();
    g_vMousePos[1] = e->pos().y();

    if(e->button() == Qt::LeftButton)
        g_leftButton = 1;
    else if(e->button() == Qt::MidButton)
        g_MidButton = 1;
    else if(e->button() == Qt::RightButton)
        g_rightButton = 1;
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    int mouseDeltaX = e->pos().x() - g_vMousePos[0];
    int mouseDeltaY = e->pos().y() - g_vMousePos[1];

    if(g_leftButton){
        static double factor = 0.1;
        camera->ZoomIn(cameraRadius * factor * mouseDeltaY);
    }
    else if(g_rightButton){
        static double factor = 0.2;
        camera->MoveRight(factor * mouseDeltaX);
        camera->MoveUp(factor * mouseDeltaY);
    }

    g_vMousePos[0] = e->pos().x();
    g_vMousePos[1] = e->pos().y();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *)
{
    g_leftButton = g_MidButton = g_rightButton = 0;
}
