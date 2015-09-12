
#include "lighting.h"
#include <stdio.h>
#include <memory.h>

Lighting::Lighting(float a_ambientIntensity, bool a_localView, bool a_twoSideLight)
    :m_ambientIntensity(a_ambientIntensity),b_localView(a_localView),b_twoSideLight(a_twoSideLight)
{
    GLfloat aGa[] = {a_ambientIntensity,a_ambientIntensity,a_ambientIntensity,1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,aGa);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,b_localView);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,b_twoSideLight);

    // Allocate memory
    for(int i=0; i<8; i++){
        SingleLight *lit = new SingleLight;
        lightList.push_back(lit);}
}

Lighting::~Lighting()
{
    for(int i=0; i<8; i++)
        delete lightList[i];
    lightList.clear();
}

void Lighting::LightScene()
{
    for(unsigned int i=0; i<lightList.size(); i++){
        if(lightList[i]->b_Enabled){
            // 0x4000 means GL_LIGHT0
            glEnable(0x4000+i);
            glLightfv(0x4000+i, GL_POSITION, lightList[i]->lightPosition);
            glLightfv(0x4000+i, GL_AMBIENT, lightList[i]->lKa);
            glLightfv(0x4000+i, GL_DIFFUSE, lightList[i]->lKd);
            glLightfv(0x4000+i, GL_SPECULAR, lightList[i]->lKs);
        }
        else
           glDisable(0x4000+i);
    }
    glEnable(GL_LIGHTING);
}

void Lighting::ModifyLight(int lightID, float x, float y, float z, float dir, float intensity, bool Enabled)
{
    if(lightID < 0 || lightID > 7)
        printf("light id out of range!\n");

    lightList[lightID]->lightID = lightID;
    lightList[lightID]->b_Enabled = Enabled;

    lightList[lightID]->lightPosition[0] = x;
    lightList[lightID]->lightPosition[1] = y;
    lightList[lightID]->lightPosition[2] = z;
    lightList[lightID]->lightPosition[3] = dir;
    lightList[lightID]->lKa[0] = lightList[lightID]->lKa[1] = lightList[lightID]->lKa[2] = intensity;
    lightList[lightID]->lKd[0] = lightList[lightID]->lKd[1] = lightList[lightID]->lKd[2] = intensity;
    lightList[lightID]->lKs[0] = lightList[lightID]->lKs[1] = lightList[lightID]->lKs[2] = intensity;

    lightList[lightID]->lKa[3] = lightList[lightID]->lKd[3] = lightList[lightID]->lKs[3] = 1.0f;

    if(glGetError() != GL_NO_ERROR)
        printf("Warning: error in the OpenGL state at the lighting turnon");
}
