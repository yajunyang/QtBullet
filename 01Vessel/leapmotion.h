#ifndef LEAPMOTION_H
#define LEAPMOTION_H
#include <Windows.h>
#include "Leap.h"
#include <GL/glu.h>

using namespace Leap;

extern Leap::Vector handPosition;

class LeapMotion  : public Listener
{
public:
    LeapMotion();

    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onFrame(const Controller&);

    void RendLeap();

protected:
    GLUquadricObj *m_sphere;
};

#endif // LEAPMOTION_H
