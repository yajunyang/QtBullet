#include "leapmotion.h"
#include <iostream>

int leapIndex = 0;
double leapRadius = 0.5;
Leap::Vector handPosition;

LeapMotion::LeapMotion()
{
    m_sphere  = gluNewQuadric();
}

void LeapMotion::onInit(const Controller &controller)
{
    if(!controller.isConnected())
        std::cout<<"Connection error"<<std::endl;
}

void LeapMotion::onConnect(const Controller &)
{
    std::cout<<"Connected"<<std::endl;
}

void LeapMotion::onFrame(const Controller &controller)
{
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();

    Leap::Hand hand = frame.hands().rightmost();
    handPosition = hand.palmPosition() * 0.02;
    handPosition.y -= 2;

    FingerList fingersInFrame = frame.fingers().extended();
    if(fingersInFrame.count() < 5)
       leapIndex = 4;
    else
       leapIndex = 0;

    static bool working = false;
    if(!working){
        std::cout<<"Leap Motion Working Now"<<std::endl;
        //std::cout<<"x="<<handPosition.x<<",y="<<handPosition.y<<",z="<<handPosition.z<<std::endl;
    working = true;
    }
}

GLfloat mat_sp[]={0.8,0.8,0.8,1.0};
GLfloat mat_sh[]={128.0};
void LeapMotion::RendLeap()
{
    glPushMatrix();
    glTranslatef(handPosition.x,handPosition.y,handPosition.z);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_sp);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_sh);
    glColor3f(0.6f,0.6f,0.6f);
    gluSphere(m_sphere,leapRadius,32,32);
    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}
