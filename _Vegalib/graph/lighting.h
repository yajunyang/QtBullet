#ifndef LIGHTING_H
#define LIGHTING_H
#include <Windows.h>

#include "GL/gl.h"
#include <vector>

struct SingleLight{
    int lightID;
    bool b_Enabled;
    float lightIntensity;
    GLfloat lightPosition[4];
    float lKa[4], lKd[4], lKs[4];

    SingleLight(){
        b_Enabled = false;
    }
} ;


class Lighting
{
public:
    //Lighting(){}
    Lighting(float a_ambientIntensity = 0.2f, bool a_localView = true, bool a_twoSideLight = true);
    ~Lighting();

    void InitLight();
    void LightScene();

    SingleLight* GetLightPtr(int id){
        return lightList[id];
    }

    void ModifyLight(int lightID, float x=0.0f, float y=0.0f, float z=0.0f, float dir = 1.0f, float intensity= 0.8f,  bool Enabled = true);
    //////////////////////////Light-configuration////////////////////////
private:

    float m_ambientIntensity;

    bool b_localView, b_twoSideLight;

    std::vector<SingleLight*> lightList;
    /////////////////////////////////////////////////////////////////////
};

#endif // LIGHTING_H
