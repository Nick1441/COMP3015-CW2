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
#include "helper/noisetex.h"
#include "helper/particleutils.h"
#include "helper/frustum.h"
#include "helper/random.h"
#include "helper/torus.h"

//#include "helper/glutils.h"

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram Shader_WireFrame;
    GLSLProgram Shader_Silhouete;
    GLSLProgram Shader_Clouds;
    GLSLProgram Shader_Wood;
    GLSLProgram Shader_Mix1_Volume;
    GLSLProgram Shader_Mix1_Comp;
    GLSLProgram Shader_Mix1_Render;
    GLSLProgram Shader_NightVision;
    GLSLProgram Shader_Gaussin;
    GLSLProgram Shader_Mix2;
    GLSLProgram Shader_EdgeDetection;
    GLSLProgram Shader_SmokEffect;
    GLSLProgram Shader_CustomParticle;


    glm::vec4 lightPos;
    GLuint SSBackground;
    //Creating Skybox Texture & Mesh
    SkyBox sky;
    GLSLProgram SkyBox;

    //Angles Used for Objects Rotation
    float angle_1, angle_2, angle_3;
    float tPrev, tPrev2;
    float rotSpeed;

    //Values for Changing Colours.
    float Value, Value2, Value3;
    bool show_demo_window = true;
    bool show_another_window = false;
    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //ImVec4 clear_color = ImColor(114, 144, 154);
    //vec4 clear_color = vec4(114, 144, 154, 1.0);

    //For mix1
    GLuint colorDepthFBO, fsQuad;
    GLuint spotTex, brickTex;

    glm::vec3 CameraPos;

    bool show_Open_File = false;
    bool show_transform = false;
    bool show_Scale = false;
    bool show_Rotation = false;
    bool show_Wireframe = false;
    bool show_Silhouette = false;
    bool show_Clouds = false;
    bool show_Wood = false;
    bool show_Mix1 = true;
    bool show_LightPos = false;
    bool show_CamPos = false;
    bool show_NightVision = false;
    bool show_Gaussin = false;
    bool show_Mix2 = false;
    bool show_edgeDetection = false;
    bool show_particleEffects = false;
    bool show_customParticle = false;
    bool show_skyBox = false;

    void ShowGaussinWindow();
    int Mix2NewPLife = 8;
    int Mix2NewPAmount = 100;
    float Mix2AccelAmount = -0.4f;
    vec3 Mix2LightCol = vec3(0.2f, 0.5, 0.9f);
    int Sil_Levels = 10;
    vec3 Sil_LightColor = vec3(0.0f, 0.83f, 0.63f);
    bool AdjLoaded = true;

    float GaussingBlurAmount = 10.0f;

    bool error_Open_File = false;

    bool include_Normal = false;


    bool toonShadingActive = true;
    int ToonOrNot;

    //Used for Opening Files. Kept here due to being used Elsewhere.

    string ObjLocation;
    string TextureLocation;

    bool FirstTimeBool = false;

    char fileInput[64];
    char fileInputTex[64];
    bool NewFileLoad = false;
    //Setting Models.
    Plane plane;                                //Plane for Base
    unique_ptr<ObjMesh> CarModel;               //Model Of Car - Texture
    unique_ptr<ObjMesh> CarModelNormal;         //Model Of Car - Normal Mapping

    unique_ptr<ObjMesh> NewCarModelNormal;
    unique_ptr<ObjMesh> NewCarModel;
    GLuint quad;

    float deltaT, time;

    unique_ptr<ObjMesh> Current;
    unique_ptr<ObjMesh> FirstSil;
    unique_ptr<ObjMesh> FirstNorm;
    bool FirstModelLoad = true;


    string CurrentTex;
    GLuint CurrentTextureGL;


    bool Light_AutoRotate = false;
    float LRF = 0;


    unique_ptr<ObjMesh> Mix1_Plane;


    bool SmokeFireTransform = false;



    glm::mat4 viewport;

    bool NormalInclude = false;
    void setMatrices6(GLSLProgram& prog);

    void setMatricesSky();


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

    //IMGUI Windows
    void ShowTransformWindow();
    void ShowScaleWindow();
    void ShowRotationWindow();
    void ShowWireframeWindow();
    void ShowSilhouetteWindow();
    void ShowCloudsWindow();
    void ShowWoodWindow();
    void ShowSmokeFireWindow();

    void TestWindow();
    void Help(const char* desc);

    void GUIMenuRender();
    void GUIWindowRender();

    void ShowOpenFile();

    void Mix1_init();
    void Mix1_render();
    void Mix1_drawscene(GLSLProgram& prog, bool onlyShadowCasters);
    void Mix1_pass1();
    void Mix1_pass2();
    void Mix1_pass3();
    void Mix1_setUpFBO();
    //void Mix1_changeObj();
    void ShowMix1Window();
    bool Mix1_bool = true;

    //Shadow Volume Info
    void Shadow_init();
    void Shadow_render();

    //Wireframe Info
    void Wireframe_init();
    void Wireframe_render();

    //Silhouette Info
    void Silhouette_init();
    void Silhouette_render();

    // ---BACKGROUNDS--
    //Cloud Background
    void Cloud_init();
    void Cloud_render();

    //Wood Grain Background
    void Wood_init();
    void Wood_render();
    void ShowMix2Window();

    void ShowLightPosWindow();

    void ResetMenu(bool on);
    void GeneralRender(GLSLProgram& prog);

    void LoadObject();
    void LoadObjectAdj();

    //Night Vision
    void NightVision_init();
    void NightVision_render();
    void NightVision_setUpFBO();
    void NightVision_pass1();
    void NightVision_pass2();

    GLuint NVrenderFBO;
    GLuint NVrenderTex;
    GLuint NVnoiseTex;
    GLuint NVpass1Index, NVpass2Index, NVQuad;


    //Gaussin
    void Gaussin_init();
    void Gaussin_render();
    void Gaussin_pass1();
    void Gaussin_pass2();
    void Gaussin_pass3();
    float Gaussin_calcG(float x, float sigma2);
    void Gaussin_setUpFBO();

    GLuint GrenderFBO, GIntermediateFBO;
    GLuint GrenderTex, GIntermediateTex;
    GLuint GQuad;




    void OffSwitcher(int i);

    //TESTING
    void Mix2_init();
    void Mix2_render();
    void Mix2_draw();
    void Mix2_setUpFBO();
    void Mix2_buildJitter();
    float Mix2_jitterCalc();
    void Mix2_initBuffers();
    void Mix2_particleRender(GLSLProgram& prog);

    GLuint Mix2_shadowFBO, Mix2_pass1Index, Mix2_pass2Index;
    Torus torus;
    int Mix2_samplesU, Mix2_samplesV;
    int Mix2_jitterMapSize;
    float Mix2_radius;
    int Mix2_shadowMapWidth, Mix2_shadowMapHeight;

    glm::mat4 Mix2_lightPV, Mix2_shadowScale;
    glm::vec3 lightpos;

    Frustum lightFrustum;
    GLuint Mix2_posBuf[2], Mix2_velBuf[2], Mix2_age[2], Mix2_rotation[2];

    GLuint Mix2_particleArray[2];

    GLuint Mix2_feedback[2];
    int Mix2_drawBuf;

    glm::vec3 Mix2_emitterPos, Mix2_emitterDir;

    int Mix2_nParticles;
    float Mix2_particleLifetime;


    //Edge Detection
    void EdgeDetection_init();
    void EdgeDetection_render();
    void EdgeDetection_pass1();
    void EdgeDetection_pass2();
    void EdgeDetection_setUpFBO();

    GLuint EDquad;
    GLuint EDfboHandle;
    GLuint EDrenderTex;


    //Snoke Particle Effectrs!
    void ParticleEffects_init();
    void ParticleEffects_initBuffers();
    void ParticleEffects_render();

    Random rand;

    glm::vec3 PEemitterPos, PEemitterDir;

    GLuint PEposBuf[2], PEvelBuf[2], PEage[2];

    GLuint PEparticleArray[2];

    GLuint PEfeedback[2];

    GLuint PEdrawBuf;

    int PEnParticles;
    float PEparticleLifetime;
    float SmokeFireTrans[3] = { -1.0f, -1.5f, -3.0f };
    float smokeLeft = -2.0f;
    float smokeRight = 2.0f;
    bool FireOrSmoke = true;


    //CUSTOM PARTICLE SYSTEM
    void CustomParticle_init();
    void CustomParticle_render();
    void CustomParticle_initBuffers();

    glm::vec3 CPemitterPos, CPemitterDir;

    GLuint CPposBuf[2], CPvelBuf[2], CPage[2], CProtation[2];

    GLuint CPparticleArray[2];
    GLuint CPfeedback[2];

    int CPnParticles;
    float CPparticleLifetime;
    int CPdrawBuf;
};

#endif // SCENEBASIC_UNIFORM_H