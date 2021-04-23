#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>

//Helper Files
#include "helper/glslprogram.h"
#include "helper/texture.h"
#include "helper/stb/stb_image.h"
//#include "helper/glutils.h"
#include "helper/skybox.h"

//Additional Files
#include "Additional Files/drawable.h"
#include "Additional Files/trianglemesh.h"
#include "Additional Files/aabb.h"
#include "Additional Files/utils.h"
#include "Additional Files/objmesh.h"
#include "Additional files/plane.h"


//#include "helper/glutils.h"

#include <glm/glm.hpp>

using namespace std;

class SceneBasic_Uniform : public Scene
{
private:
    //Creating Each Shader Program.
    GLSLProgram Shader_1;
    GLSLProgram Shader_2;
    GLSLProgram Shader_3;
    GLSLProgram Shader_4;
    GLSLProgram Shader_5;

    //Creating Skybox Texture & Mesh
    SkyBox sky;
    GLSLProgram SkyBox;

    //Angles Used for Objects Rotation
    float angle_1, angle_2, angle_3;
    float tPrev, tPrev2;
    float rotSpeed;

    //Values for Changing Colours.
    float Value, Value2, Value3;
    bool show_test_window = true;
    bool show_another_window = false;

    //ImVec4 clear_color = ImColor(114, 144, 154);
    //vec4 clear_color = vec4(114, 144, 154, 1.0);


    //Setting Models.
    Plane plane;                                //Plane for Base
    unique_ptr<ObjMesh> CarModel;               //Model Of Car - Texture
    unique_ptr<ObjMesh> CarModelNormal;         //Model Of Car - Normal Mapping

    //Set Matricies For Each Shader.
    void setMatrices();
    void setMatrices2();
    void setMatrices3();
    void setMatrices4();
    void setMatrices5();

    void setMatricesSky();

    //This manages all of the non changing variables which can be set in Init.
    void LoadShadersVariables();

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

    //Checking for User Input/Rendering Skybox.
    void InputPressed(int);
    void RenderSkyBox();

    //Methods for Each Scenario.
    void Scenario_1();
    void Scenario_2();
    void Scenario_3();
    void Scenario_4();
    void Scenario_5();
};

#endif // SCENEBASIC_UNIFORM_H