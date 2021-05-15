#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

//Helper Files
#include "helper/skybox.h"
#include "helper/texture.h"
#include "helper/glslprogram.h"
#include "helper/stb/stb_image.h"

//Additional Files
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "helper/scene.h"
#include "helper/torus.h"
#include "helper/random.h"
#include "helper/frustum.h"
#include "helper/noisetex.h"
#include "helper/particleutils.h"
#include "Additional Files/aabb.h"
#include "Additional files/plane.h"
#include "Additional Files/utils.h"
#include "Additional Files/objmesh.h"
#include "Additional Files/drawable.h"
#include "Additional Files/trianglemesh.h"

using namespace std;
using namespace glm;

class SceneBasic_Uniform : public Scene
{
private:
    //  --- Shaders Used Throughout Program ---
    GLSLProgram Shader_Mix1_Comp;
    GLSLProgram Shader_Mix1_Render;
    GLSLProgram Shader_Mix1_Volume;         //Mix of Silhouette + Shadow Volumes

    GLSLProgram Shader_Mix2;                //Mix of Shadows Soft Edges + Particle System

    GLSLProgram Shader_SkyBox;
    GLSLProgram Shader_Gaussin;
    GLSLProgram Shader_WireFrame;
    GLSLProgram Shader_Silhouete;
    GLSLProgram Shader_SmokEffect;          //Smoke and Fire Particle Systems!
    GLSLProgram Shader_NightVision;
    GLSLProgram Shader_EdgeDetection;
    GLSLProgram Shader_CustomParticle;      //Custom Particle Effect of .Obj's!

    GLSLProgram Shader_Wood;
    GLSLProgram Shader_Clouds;              //Backgrounds, Clouds and Smoke, Using Noise!

    //  --- Bools for Opening Windows/Shaders ---
    bool show_Mix1 = true;                  //True as initial Shader.
    bool show_Mix2 = false;
    bool show_Open_File = false;

    bool show_Gaussin = false;
    bool show_Wireframe = false;
    bool show_Silhouette = false;
    bool show_NightVision = false;
    bool show_edgeDetection = false;
    bool show_customParticle = false;
    bool show_particleEffects = false;

    bool show_Wood = false;
    bool show_skyBox = false;
    bool show_Clouds = false;
    
    bool show_LightPos = false;
    
    bool show_Scale = false;
    bool show_Rotation = false;
    bool show_transform = false;


    //  --- General ---
   
    float angle_1;                          //Used for Rotation throughout.vec3 CameraPos;                         //Camera Pos Used throughout so will remain constant.
    vec4 lightPos;                          //Light Pos used throughout so will remain constant.
    bool AdjLoaded = true;                  //When loading objects, to see what Load type is current
    
    string ObjLocation;                     //Stores the Currenlty Loaded Obj Name  
    string TextureLocation;                 //Stores the Currently loaded Texture Name
    unique_ptr<ObjMesh> Current;            //Current Object laoded. Used throughout.
    GLuint CurrentTextureGL;            
            
    float LRF = 0;                          //Time for Auto Rotate
    bool Light_AutoRotate = false;          //Bool for auto rotate, used throughout.
    float ObjTransform[3] = { 0.0f, 0.0f, 0.0f };   //Easier to have this for GUI then set to vec
    
    mat4 viewport;                          //Used for Shader.

    vec3 ObjTransformVec;                   //Vector for Objects Transform.
    float ObjScale = 0.8f;                  //Obj Scale, Used throghout.
    float ObjRotationX = 0.0f;              //Rotation, Could have used [3] like transform.
    float ObjRotationY = 0.0f;
    float ObjRotationZ = 0.0f;

    bool AutoRotate = false;                //Objects Auto Rotate Feature
    float AutoRotateSpeed = 8.0f;

    vec3 CameraPos;                         //Camera Pos, Used throughout so all shaders are same view.
    float LightAngle = 1.0f;                //Light Angle, Changed through GUI
    

    //  --- Mix 1 - Shadow Volumes + Silhouette ---
    GLuint SSBackground;                    //Texture Background for .Obj
    GLuint colorDepthFBO, fsQuad;           //FBO for Mix1
    
    int ToonOrNot;                          //Used for Changing to Toon inside Frag.
    bool toonShadingActive = true;          //Bool which can be checked for Toon.
    
    unique_ptr<ObjMesh> Mix1_Plane;         //Shadows Render on .Obj not Planes, So Upgraded plane.
    
    int Mix1Levels = 10;                    //Custom Variables for real time render changes.
    float Mix1PE = 0.00f;
    float Mix1EW = 0.004f;
    vec3 Mix1Color = vec3(1.0f);

    //  --- Mix 2 - Shadow Soft Edges + Particle Systems ---
    int Mix2NewPLife = 8;                   //Variables for Setting new Variables in GUI
    int Mix2NewPAmount = 100;
    float Mix2AccelAmount = -0.4f;
    vec3 Mix2LightCol = vec3(0.2f, 0.5, 0.9f);

    float time;                             //Time and DeltaT used for Duration of System
    Plane plane;                            //Used as ground to show shadows.
    Torus torus;                            //Used shape for System.
    float deltaT;
    
    GLuint Mix2_shadowFBO;                  //FBO for Shadow
    GLuint Mix2_pass1Index;                 
    GLuint Mix2_pass2Index;

    float Mix2_radius;                      //Jitter Map Settings.
    int Mix2_samplesU;
    int Mix2_samplesV;
    int Mix2_jitterMapSize;
    
    int Mix2_drawBuf;
    int Mix2_shadowMapWidth;                //Sizes for Shadow Map
    int Mix2_shadowMapHeight;

    vec3 lightpos;                          //Used in Mix2, Differnt to Default
    mat4 Mix2_lightPV;
    mat4 Mix2_shadowScale;

    Frustum lightFrustum;                   //For calculating Shadow

    GLuint Mix2_age[2];                     //Feedback from System
    GLuint Mix2_posBuf[2];
    GLuint Mix2_velBuf[2];
    GLuint Mix2_rotation[2];
    GLuint Mix2_particleArray[2];           //Holds Particles Info

    vec3 Mix2_emitterPos;                   //Pos/Direction of Oragin of System.
    vec3 Mix2_emitterDir;
    GLuint Mix2_feedback[2];

    int Mix2_nParticles;                    //Particle amount/Lifetime!
    float Mix2_particleLifetime;

    //  --- Smoke Particle Effects ---
    Random rand;                            //Uses Random for ...well... randomnes....
    bool SmokeFireTransform = false;        //Enables transform for Particle Effect
    glm::vec3 PEemitterPos, PEemitterDir;

    GLuint PEposBuf[2], PEvelBuf[2], PEage[2];  //Feedback for Particle Effects
    GLuint PEparticleArray[2];
    GLuint PEfeedback[2];
    GLuint PEdrawBuf;

    int PEnParticles;                           //Particle Amount, Can be changed in GUI
    float PEparticleLifetime;                   //Particle Lifetime, Can be changed in GUI

    float SmokeFireTrans[3] = { -1.0f, -1.5f, -3.0f };  //Default Transform for Particles.
    float smokeLeft = -2.0f;                    //Left Width of Effect, Can be changed!
    float smokeRight = 2.0f;                    //Right Width of Effect, Can be Changed!
    bool FireOrSmoke = true;                    //Setting it as fire or smoke!

    //  --- Wire Frame ---
    float ObjWFWidth = 0.75f;                      //Variables for Customising Shader.   
    vec4 ObjWFBack = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 ObjWFColor = vec4(0.05, 0.0f, 0.05f, 1.0f);


    //  --- Silhouette ---
    int Sil_Levels = 10;                    //Setting Default Levels for Toon
    vec3 Sil_LightColor = vec3(0.0f, 0.83f, 0.63f);
    float Sil_EdgeWidth = 0.004f;
    float Sil_PctExtend = 0.0;
    vec4 Sil_LineColor = vec4(0.05f, 0.0f, 0.05f, 1.0f);

    //  --- SkyBox ---
    SkyBox sky;                             //Setting SkyBox... All is required..

    //  --- Gaussin ---
    float GaussingBlurAmount = 10.0f;       //Blur amount for Gaussin!
    GLuint GrenderFBO, GIntermediateFBO;    //FBO's for Rendering   
    GLuint GrenderTex, GIntermediateTex;
    GLuint GQuad;                           //Quad to render to!

    //  --- File Loading ---
    char fileInput[64];                     //Objects File Name From GUI
    char fileInputTex[64];                  //Textures File Name From GUI
    bool NewFileLoad = false;               //Updates Info when a new file is added.

    //  --- Cloud Background ---
    GLuint quad;                                    //Quad for Rendering on!
    vec4 Clouds_Sky = vec4(0.1, 0.3, 0.9, 1.0);     //Colours for changing background!
    vec4 Clouds_Cloud = vec4(1.0, 1.0, 1.0, 1.0);

    //  --- Wood Effect Background ---
    vec4 DarkWoodColor = vec4(0.8, 0.5, 0.1, 1.0);  //Colours for changing background!
    vec4 LightWoodColor = vec4(1.0, 0.75, 0.25, 1.0);

    //  --- Night Vision ---        
    GLuint NVrenderFBO;                     //FBO's for Render
    GLuint NVrenderTex;                     //GLuints for Noise Texture
    GLuint NVnoiseTex;
    GLuint NVpass1Index, NVpass2Index, NVQuad;  //For index passing sub routines.

    //  --- Edge Detection ---
    GLuint EDquad;                          //FBO's Texture and handles for Detection.
    GLuint EDfboHandle;
    GLuint EDrenderTex;

    //  --- Custom Object Particle System
    vec3 CPemitterPos;                      //Direction and Pos of Emmiter.
    vec3 CPemitterDir;

    GLuint CPfeedback[2];
    GLuint CPparticleArray[2];
    GLuint CPposBuf[2], CPvelBuf[2], CPage[2], CProtation[2];   //Feedback for Particles.
    
    int CPdrawBuf;                          //Draw Buffer for Particle Effects
    int CPnParticles;                       //Particle Amount & Lifetime.
    float CPparticleLifetime;

    //  --- Basic SceneBasic Info ---
    void setMatrices(GLSLProgram& prog);    
    void compile();

public:
    SceneBasic_Uniform();

    //  --- Basic SceneBasic Info ---
    void render();
    void initScene();
    void update(float t);
    void resize(int, int);


    //  --- ImGUI Windows ---               //Used for showing Each GUI window
    void ShowScaleWindow();
    void ShowRotationWindow();
    void ShowTransformWindow();

    void ShowOpenFile();
    void GUIMenuRender();
    void ShowMix2Window();
    void ShowWoodWindow();
    void ShowMix1Window();
    void GUIWindowRender();
    void ShowCloudsWindow();
    void ShowGaussinWindow();
    void ShowLightPosWindow();
    void ShowWireframeWindow();
    void ShowSmokeFireWindow();
    void ShowSilhouetteWindow();
    void showCustomPrticleWindow();

    //  --- General Render ---              //Used for multiple uses of same rendering.
    void GeneralRender(GLSLProgram& prog);

    //  --- Mix 1 Methods ---
    void Mix1_init();
    void Mix1_render();
    void Mix1_pass1();
    void Mix1_pass2();
    void Mix1_pass3();
    void Mix1_setUpFBO();
    void Mix1_drawscene(GLSLProgram& prog, bool onlyShadowCasters);
    
    //  --- Mix 2 Methods ---
    void Mix2_init();
    void Mix2_render();
    void Mix2_draw();
    void Mix2_setUpFBO();
    void Mix2_buildJitter();
    float Mix2_jitterCalc();
    void Mix2_initBuffers();
    void Mix2_particleRender(GLSLProgram& prog);

    //  --- Wireframe Info ---
    void Wireframe_init();
    void Wireframe_render();

    //  --- Silhouette Info ---
    void Silhouette_init();
    void Silhouette_render();

    //  --- Cloud Background ---
    void Cloud_init();
    void Cloud_render();

    //  --- Wood Grain Background ---
    void Wood_init();
    void Wood_render();

    //  --- Night Vision  ---
    void NightVision_init();
    void NightVision_pass1();
    void NightVision_pass2();
    void NightVision_render();
    void NightVision_setUpFBO();

    //  --- Gaussin ---
    void Gaussin_init();
    void Gaussin_pass1();
    void Gaussin_pass2();
    void Gaussin_pass3();
    void Gaussin_render();
    void Gaussin_setUpFBO();
    float Gaussin_calcG(float x, float sigma2);

    //  --- Edge Detection  ---
    void EdgeDetection_init();
    void EdgeDetection_pass1();
    void EdgeDetection_pass2();
    void EdgeDetection_render();
    void EdgeDetection_setUpFBO();

    //  --- Smoke/Fire Particles ---
    void ParticleEffects_init();
    void ParticleEffects_render();
    void ParticleEffects_initBuffers();

    //  --- Custom Particle System ---
    void CustomParticle_init();
    void CustomParticle_render();
    void CustomParticle_initBuffers();

    //  --- General Rendering  ---
    void LoadObject();                          //Loads object if needed
    void LoadObjectAdj();                       //Loads object with ADJ if needed
    void OffSwitcher(int i);                    //Used when Toggeling Shaders GUI
    void RenderSkyBox();
};

#endif // SCENEBASIC_UNIFORM_H