#include "scenebasic_uniform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iostream>

#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_opengl3.h"
#include "imGUI/imgui_impl_glfw.h"



using std::cerr;
using std::endl;

using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;

//Used for Switching Scenes/Toggles./-
int OverallLoad = 1;
bool Scene1Toggle = true;
bool Scene4Toggle = false;

//Objects Position
float ObjX = 0.0f;
float ObjY = 0.0f;
float ObjZ = 0.0f;

//Objects Scale
float ObjScale = 0.8f;

//Objects Rotation
float ObjRotationX = 0.0f;
float ObjRotationY = 0.0f;
float ObjRotationZ = 0.0f;
bool AutoRotate = false;
float AutoRotateSpeed = 50.0f;

//Objects Wire Frame
float ObjWFWidth = 0.75f;
vec4 ObjWFColor = vec4(0.05, 0.0f, 0.05f, 1.0f);
vec4 ObjWFBack = vec4(1.0f, 1.0f, 1.0f, 1.0f);


//Setting Components Used Throughout Project
SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100), angle_2(0.0f), angle_1(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), sky(100.0f), Current(), CarModel(), CarModelNormal()
{
    //Loading In Models, Just Texture & Normal Mapping Models.
    CarModel = ObjMesh::load("media/SportsCar.obj", false);
    CarModelNormal = ObjMesh::load("media/SportsCar.obj", false, true);
    Current = ObjMesh::load("media/Dice.obj", false, true);
    //Current = ObjMesh::load("media/bs_ears.obj", false);
}

void SceneBasic_Uniform::initScene()
{
    
    compile();                                                                              //Compile all shaders.
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    //projection = mat4(1.0f);                                                                //Default Projection Mat
    angle_2 = glm::radians(90.0f);                                                          //Defualting Angle to Start Rotation of Camera.

    //Loading in Textures.
    GLuint ID1 = Texture::loadTexture("media/texture/skylineColor.png");
    GLuint ID2 = Texture::loadTexture("media/texture/skylineNormal.png");
    GLuint ID3 = Texture::loadTexture("media/texture/moss.png");
    GLuint ID4 = Texture::loadTexture("media/texture/skylineColor2.png");
    GLuint ID5 = Texture::loadTexture("media/texture/skylineColor3.png");

    GLuint cubeTex = Texture::loadCubeMap("media/texture/cube/Skybox/skybox2");

    //Loading each Texture into Bindings.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ID2);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ID3);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ID4);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ID5);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    LoadShadersVariables();

    WireFramePre();
}

void SceneBasic_Uniform::compile()
{
    //Links Each shader to each file and sets up for use.

    //Scenario 1 Shader
    try {
        Shader_1.compileShader("shader/Scenario1.vert");
        Shader_1.compileShader("shader/Scenario1.frag");
        Shader_1.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    //Scenario 2 Shader
    try {
        Shader_2.compileShader("shader/Scenario2.vert");
        Shader_2.compileShader("shader/Scenario2.frag");
        Shader_2.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    //Scenario 3 Shader
    try {
        Shader_3.compileShader("shader/Scenario3.vert");
        Shader_3.compileShader("shader/Scenario3.frag");
        Shader_3.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    //Scenario 4 Shader
    try {
        Shader_4.compileShader("shader/Scenario4.vert");
        Shader_4.compileShader("shader/Scenario4.frag");
        Shader_4.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    //Scenario 5 Shader
    try {
        Shader_5.compileShader("shader/Scenario5.vert");
        Shader_5.compileShader("shader/Scenario5.frag");
        Shader_5.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    //SkyBox Shader
    try {
        SkyBox.compileShader("shader/SkyBox.vert");
        SkyBox.compileShader("shader/SkyBox.frag");
        SkyBox.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    try {
        Shader_WireFrame.compileShader("shader/Wireframe/Wireframe.vert");
        Shader_WireFrame.compileShader("shader/Wireframe/Wireframe.frag");
        Shader_WireFrame.compileShader("shader/Wireframe/Wireframe.geom");
        Shader_WireFrame.link();
        Shader_WireFrame.use();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    //Angle 1 Rotation, for scene 1 Lighting.
    if (m_animate) {
        angle_1 += 0.05f;
        if (angle_1 >= 360.0f)
            angle_1 -= 360.0f;
    }

    //SkyBox Rotation
    float deltaT2 = t - tPrev;
    if (tPrev == 0.0f)
    {
        deltaT2 = 0.0f;
    }

    tPrev = t;
    angle_2 += rotSpeed * deltaT2;

    if (angle_2 > glm::two_pi<float>())
    {
        angle_2 -= glm::two_pi<float>();
    }

    //Rotation for Light Moving.
    if (m_animate) {
        angle_3 += 0.015f;
        if (angle_3 >= 360.0f)
            angle_3 -= 360.0f;
    }

    //Used to change the colour. (Each offset to mix it up)
    float timeValue = t;
    Value = sin(timeValue) / 4.0f + 0.5f;
    Value2 = sin(timeValue) / 4.0f + 0.25f;
    Value3 = sin(timeValue) / 4.0f + 0.75f;
}

void SceneBasic_Uniform::render()
{
    //
    //Overall Controller for project. Changes scene based on OverallLoad Integer. 
    //Changes each methods to render & Renders Skybox.
    //

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    RenderGUI();
    RenderGUIChecker();

    if (show_Wireframe)
    {
        
        WireFrame();
    }
    else
    {
        TestWindow();
        RenderSkyBox();
    }
    
    //WireFrame();

    //if (OverallLoad == 1)
    //{
    //    Scenario_1();
    //    RenderSkyBox();
    //}
    //else if (OverallLoad == 2)
    //{
    //    Scenario_2();
    //    RenderSkyBox();
    //}
    //else if (OverallLoad == 3)
    //{
    //    Scenario_3();
    //    RenderSkyBox();
    //}
    //else if (OverallLoad == 4)
    //{
    //    Scenario_4();
    //    RenderSkyBox();
    //}
    //else if (OverallLoad == 5)
    //{
    //    Scenario_5();
    //    RenderSkyBox();
    //}
    //This is what renders the GUI


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


}

//Setting Matrices for Each Shader. All The same code, just differnt Shaders.
void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model;

    Shader_1.setUniform("ModelViewMatrix", mv);
    Shader_1.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    Shader_1.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices2()
{
    mat4 mv = view * model;

    Shader_2.setUniform("ModelViewMatrix", mv);
    Shader_2.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    Shader_2.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices3()
{
    mat4 mv = view * model;

    Shader_3.setUniform("ModelViewMatrix", mv);
    Shader_3.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    Shader_3.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices4()
{
    mat4 mv = view * model;

    Shader_4.setUniform("ModelViewMatrix", mv);
    Shader_4.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    Shader_4.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices5()
{
    mat4 mv = view * model;

    Shader_5.setUniform("ModelViewMatrix", mv);
    Shader_5.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    Shader_5.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatrices6()
{
    mat4 mv = view * model;

    Shader_WireFrame.setUniform("ModelViewMatrix", mv);
    Shader_WireFrame.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));


    Shader_WireFrame.setUniform("MVP", projection2 * mv);
    Shader_WireFrame.setUniform("ViewportMatrix", viewport);
}

void SceneBasic_Uniform::setMatricesSky()
{
    mat4 mv = view * model;

    SkyBox.setUniform("ModelViewMatrix", mv);
    SkyBox.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    SkyBox.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::LoadShadersVariables()
{
    //
    //SCENE 1 NON-CHANGING VARIABLES
    Shader_1.use();

    //MULTIPLE LIGHTS CODE
    //Shader_1.setUniform("Light[0].La", vec3(0.1f, 0.1f, 0.1f));
    //Shader_1.setUniform("Light[1].La", vec3(0.9f, 0.9f, 0.9f));

    // Shader_1.setUniform("Light[0].L", vec3(1.0f, 1.0f, 1.0f));
    //Shader_1.setUniform("Light[1].L", vec3(0.0f, 0.0f, 0.0f));

    //Setting Lights Positions (Move Into Scenario1)
    //vec4 lightPos = vec4(10.0f * cos(angle_1), 20.0f, 10.0f * sin(angle_1), 6.0f);
    //Shader_1.setUniform("Light[0].Position", view * lightPos);
    //Shader_1.setUniform("Light[1].Position", vec3(50.0f, 0.0f, 0.0f));

    //Single Light Code
    Shader_1.setUniform("Light.La", vec3(0.1f, 0.1f, 0.1f));
    Shader_1.setUniform("Light.L", vec3(1.0f, 1.0f, 1.0f));

    Shader_1.setUniform("Material.Ks", 0.2f, 0.2f, 0.2f);
    Shader_1.setUniform("Material.Kd", 0.5f, 0.5f, 0.5f);
    Shader_1.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    Shader_1.setUniform("Material.Shininess", 300.0f);

    //
    //SCENE 2 NON-CHANGING VARIABLES
    Shader_2.use();

    //Setting Lights Positions
    Shader_2.setUniform("Light.Position", vec3(1.0f, 0.0f, 0.0f));
    Shader_2.setUniform("Light.La", vec3(0.1f, 0.1f, 0.1f));
    Shader_2.setUniform("Light.L", vec3(0.8f, 0.8f, 0.8f));

    //Setting Material Values for Car/Plane
    Shader_2.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    Shader_2.setUniform("Material.Ka", 0.9f, 0.9f, 0.9f);
    Shader_2.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
    Shader_2.setUniform("Material.Shininess", 180.0f);


    //SCENE 3 NON-CHANGING VARIABLES
    Shader_3.use();

    //Setting Lights Positions
    Shader_3.setUniform("Light[0].Position", vec4(-5.0f, -5.0f, 3.0f, 0.0f));
    Shader_3.setUniform("Light[1].Position", vec4(5.0f, 5.0f, 3.0f, 0.0f));
    Shader_3.setUniform("Light[2].Position", vec4(-5.0f, -5.0f, 3.0f, 0.0f));

    for (int i = 0; i < 3; i++)
    {
        //Setting Lights Ambient
        stringstream Label;
        Label << "Light[" << i << "].La";
        Shader_3.setUniform(Label.str().c_str(), vec3(0.2f, 0.2f, 0.2f));
    }

    //Setting Lights Colour
    Shader_3.setUniform("Light[0].L", vec3(0.06f, 0.34f, 0.81f));
    Shader_3.setUniform("Light[1].L", vec3(0.06f, 0.18f, 0.27f));
    Shader_3.setUniform("Light[2].L", vec3(0.78f, 0.14f, 0.06f));

    Shader_3.setUniform("Material.Ks", 0.2f, 0.2f, 0.2f);
    Shader_3.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    Shader_3.setUniform("Material.Kd", 0.2f, 0.2f, 0.2f);
    Shader_3.setUniform("Material.Shininess", 180.0f);


    //SCENE 5 NON-CHANGING VARIABLES
    Shader_5.use();

    Shader_5.setUniform("Light.La", vec3(0.1f, 0.1f, 0.1f));
    Shader_5.setUniform("Light.L", vec3(0.2f, 0.2f, 0.2f));

    //Setting Material Values for Car/Plane
    Shader_5.setUniform("Material.Ks", 0.2f, 0.2f, 0.2f);
    Shader_5.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    Shader_5.setUniform("Material.Kd", 0.2f, 0.2f, 0.2f);
    Shader_5.setUniform("Material.Shininess", 180.0f);

    Shader_5.setUniform("Spot.La", vec3(0.2f));
    Shader_5.setUniform("Spot.Exponent", 10.0f); //Blurry Around Circle
    Shader_5.setUniform("Spot.CutOff", glm::radians(5.0f));  //Size of circle
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    projection2 = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
    viewport = mat4(vec4(w2, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, h2, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
}

void  SceneBasic_Uniform::InputPressed(int num)
{
    //Checks for input Pressed, If below 7, its changing scenes!
    if (num < 7)
    {
        OverallLoad = num;
    }
    
    //Scene 1 & 4 Camera Movement/Fog Toggle
    if (OverallLoad == 1 && num == 10)
    {
        Scene1Toggle = false;
    }
    else if (OverallLoad == 4 && num == 10)
    {
        Scene4Toggle = false;
    }

    if (OverallLoad == 1 && num == 11)
    {
        Scene1Toggle = true;
    }
    else if (OverallLoad == 4 && num == 11)
    {
        Scene4Toggle = true;
    }
}

void  SceneBasic_Uniform::Scenario_1()
{
    //
    // - BlinnPhong Shading
    // - Multiple Models
    // - Multiple Textures on differnt Objects! (Same Shader!)
    // - Normal Mapping
    // - Moving Lights!
    // - Toggable View! "(Enter/LShift)""
    //

    //Setting Details for the camera toogle. Either still or uses angle to rotate.
    vec3 cameraPos = vec3(0.0f);
    if (Scene1Toggle)
    {
        cameraPos = vec3(7.0f * cos(angle_2), 2.0f, 7.0f * sin(angle_2));
        view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    }
    else
    {
        cameraPos = vec3(5.5f * cos(90), 2.0f, 5.5f * sin(90));
        view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    }

    //Setting Shader To Use
    Shader_1.use();

    //Setting Lights Positions(Move Into Scenario1)
    vec4 lightPos = vec4(10.0f * cos(angle_1), 20.0f, 10.0f * sin(angle_1), 6.0f);
    Shader_1.setUniform("Light.Position", view * lightPos);
        
    //Setting Pass, This will ignore the texture, so it isnt on the floor!
    Shader_1.setUniform("Pass", 1);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices();
    plane.render();

    //Setting Pass 0 for Differnt Textures! Gray this one!
    Shader_1.setUniform("Pass", 0);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.0f, -0.45f, 0.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    setMatrices();
    Current->render();

    //Uses differnt pass for differnt Texture! Pink & Blue!
    Shader_1.setUniform("Pass", 2);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 1.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    setMatrices();
    Current->render();

    //Uses differnt pass for differnt Texture! Green & Pink!
    Shader_1.setUniform("Pass", 3);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, -0.45f, 2.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    setMatrices();
    Current->render();
}

void  SceneBasic_Uniform::Scenario_2()
{
    //
    // - Alpha Discarding
    // - BlinnPhong Shading
    // - Lighting
    // - Texturing Objects
    //

    //Setting View To Look At.
    view = glm::lookAt(vec3(-2.5f, 2.f, 2.0f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting Shader To Use
    Shader_2.use();

    //Sets Pass to Ignore Texture Coords method.
    Shader_2.setUniform("Pass_2", 0);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    model = glm::rotate(model, glm::radians((float)angle_1 * 6), vec3(0.0f, 1.0f, 0.0f));
    setMatrices2();
    plane.render();

    //Set Pass to Include Alpha Discarding
    Shader_2.setUniform("Pass_2", 1);

    //Render Object.
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    model = glm::rotate(model, glm::radians((float)angle_1 * 6), vec3(0.0f, 1.0f, 0.0f));
    setMatrices2();
    CarModel->render();
}

void  SceneBasic_Uniform::Scenario_3()
{
    // 
    // - BlinnPhong Shading
    // - Multiple Textures
    // - Multiple Lighting
    // - Texturing Objects
    //

    //Setting View to Look At.
    view = glm::lookAt(vec3(-2.5f, 2.f, 2.0f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting Shader To Use
    Shader_3.use();

    //Sets Pass to Ignore Texture Coords method.
    Shader_3.setUniform("Pass_3", 1);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    model = glm::rotate(model, glm::radians((float)angle_1 * 6), vec3(0.0f, 1.0f, 0.0f));
    setMatrices3();
    plane.render();

    //Sets Pass to Load Textures.
    Shader_3.setUniform("Pass_3", 0);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    model = glm::rotate(model, glm::radians((float)angle_1 * 6), vec3(0.0f, 1.0f, 0.0f));
    setMatrices3();
    CarModel->render();
}

void  SceneBasic_Uniform::Scenario_4()
{
    // 
    // - Toon Shading
    // - Spotlight
    // - Fog (May be broken on differnt Computers)
    // - Changing Colour Light!
    //

    //Toggle for Fog (Doesnt work on all Machines)
    if (Scene4Toggle)
    {
        Shader_4.setUniform("Pass", 1);
    }
    else
    {
        Shader_4.setUniform("Pass", 0);
    }

    //Set What to look At.
    view = glm::lookAt(vec3(-2.5f, 2.f, 2.0f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    
    Shader_4.use();

    Shader_4.setUniform("Light.L", vec3(Value3, Value2, Value));
    Shader_4.setUniform("Light.La", vec3(0.2f, 0.2f, 0.2f));

    //Setting Materials Setting (Doesnt Like Being Moved Out of Method.)
    Shader_4.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
    Shader_4.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    Shader_4.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);
    Shader_4.setUniform("Material.Shininess", 100.0f);
    Shader_4.setUniform("Material.Shininess", 100.0f);

    Shader_4.setUniform("Fog.MaxDist", 50.0f);
    Shader_4.setUniform("Fog.MinDist", 1.0f);
    Shader_4.setUniform("Fog.Color", vec3(0.8f, 0.8f, 0.8f));

    //Setting Shader To Use
    vec4 lightPos = vec4(10.0f * cos(angle_3), 10.0f, 10.0f * sin(angle_3), 1.0f);
    Shader_4.setUniform("Light.Position", view * lightPos);

    //Render Plane
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices4();
    plane.render();

    //Render the Car.
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    setMatrices4();
    CarModel->render();
}

void  SceneBasic_Uniform::Scenario_5()
{
    //
    // - BlinnPhong Shading
    // - Texturing
    // - Multiple Lights
    // - Spotlighting, Changes colour!
    // - Texturing Objects
    //

    //Setting where to look.
    view = glm::lookAt(vec3(-2.5f, 2.f, 2.0f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting Shader To Use
    Shader_5.use();

    //Setting Lights Positions
    vec4 lightPos = vec4(10.0f * cos(angle_1), 20.0f, 10.0f * sin(angle_1), 6.0f);
    Shader_5.setUniform("Light.Position", view * lightPos);

    //Setting Spot Light
    lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    Shader_5.setUniform("Spot.Position", vec3(view * lightPos));
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    Shader_5.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    Shader_5.setUniform("Spot.L", vec3(Value3, Value2, Value));

    //Changing pass to ignore Textures.
    Shader_5.setUniform("Pass", 0);

    //Render Plane
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    model = glm::rotate(model, glm::radians((float)angle_1 * 6), vec3(0.0f, 1.0f, 0.0f));
    setMatrices5();
    plane.render();

    //Set Pass to Include Texture
    Shader_5.setUniform("Pass", 1);

    //Render Car.
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    model = glm::rotate(model, glm::radians((float)angle_1 * 6), vec3(0.0f, 1.0f, 0.0f));
    setMatrices5();
    CarModel->render();
}

void SceneBasic_Uniform::RenderSkyBox()
{
    //Set Skybox to Current.
    SkyBox.use();

    model = mat4(1.0f);

    //Render Skybox.
    setMatricesSky();
    sky.render();
}

void SceneBasic_Uniform::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Open", "Open .obj File", &show_Open_File);
            ImGui::Separator();
            ImGui::MenuItem("Close", "Exit Program");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Object"))
        {
            ImGui::MenuItem("Transfrom", "Move Object", &show_transform);
            ImGui::MenuItem("Rotation", "Rotate Object", &show_Rotation);
            ImGui::MenuItem("Scale", "Scale Object", &show_Scale);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Shaders"))
        {
            ImGui::MenuItem("WireFrame", "View Objects Wireframe", &show_Wireframe);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void SceneBasic_Uniform::RenderGUIChecker()
{
    if (show_Open_File)
    {
        ShowOpenFile();
    }

    if (show_transform)
    {
        ShowTransformWindow();
    }

    if (show_Scale)
    {
        ShowScaleWindow();
    }

    if (show_Rotation) { ShowRotationWindow(); }
    if (show_Wireframe) { ShowWireframeWindow(); }

}

void SceneBasic_Uniform::ShowOpenFile()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoResize;
    OpenFileTags |= ImGuiWindowFlags_NoCollapse;
    OpenFileTags |= ImGuiWindowFlags_NoMove;

    static char fileInput[64] = "";
    static char textureInput[64] = "";
    static char normalInput[64] = "";
    
    {
        ImGui::Begin("Open File", &show_Open_File, OpenFileTags);
        ImGui::Text("Pace all .obj infomation into 'ImportedObj'");
        ImGui::Text("Type .obj File Name. (Including Caps)");
        ImGui::InputText(".obj - Object", fileInput, 64, ImGuiInputTextFlags_CharsNoBlank);
        ImGui::InputText(".png - Texture", fileInput, 64, ImGuiInputTextFlags_CharsNoBlank);
        ImGui::Checkbox("Includes Normal", &include_Normal);
        if (include_Normal)
        {
            ImGui::InputText(".png - Normal", fileInput, 64, ImGuiInputTextFlags_CharsNoBlank);
        }
        if (error_Open_File)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Issue Loading, Check Spelling.");
        }
        //Check Box for has Texture & Normal

        if (ImGui::Button("Open"))
        {
            string InputFileString(fileInput);
            
            if (InputFileString != "")
            {
                error_Open_File = false;

                string NewString = "media/" + InputFileString + ".obj";
                const char* NewChar = NewString.c_str();

                //This will set new File Location
                if (include_Normal == true)
                {
                    Current = ObjMesh::load(NewChar, false, true);
                    NormalInclude = true;
                    show_Open_File = false;
                }
                else
                {
                    Current = ObjMesh::load(NewChar, true);
                    NormalInclude = false;
                    show_Open_File = false;
                }
            }
            else
            {
                error_Open_File = true;
            }
        }
        ImGui::End();
    }


}

void SceneBasic_Uniform::TestWindow()
{
                                                         //Default Projection Mat

    //Setting Details for the camera toogle. Either still or uses angle to rotate.
    vec3 cameraPos = vec3(0.0f);
    cameraPos = vec3(0.0f * cos(90), 2.0f, 5.5f * sin(90));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting Shader To Use
    Shader_1.use();

    //Setting Lights Positions(Move Into Scenario1)
    vec4 lightPos = vec4(10.0f * cos(angle_1), 20.0f, 10.0f * sin(angle_1), 6.0f);
    Shader_1.setUniform("Light.Position", view * lightPos);

    //Setting Pass, This will ignore the texture, so it isnt on the floor!
    Shader_1.setUniform("Pass", 1);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices();
    plane.render();

    //Setting Pass 0 for Differnt Textures! Gray this one!
    Shader_1.setUniform("Pass", 2);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(ObjX, ObjY, ObjZ));
    model = glm::scale(model, vec3(ObjScale, ObjScale, ObjScale));

    //Rotation
    if (!AutoRotate)
    {
        model = glm::rotate(model, glm::radians(ObjRotationX * 57.5f), vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(ObjRotationY * 57.5f), vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(ObjRotationZ * 57.5f), vec3(0.0f, 0.0f, 1.0f));
    }
    else
    {
        ObjRotationX = 0.0f;
        ObjRotationY = 0.0f;
        ObjRotationZ = 0.0f;
        model = glm::rotate(model, glm::radians((float)angle_2 * AutoRotateSpeed), vec3(0.0f, 1.0f, 0.0f));

    }

    setMatrices();
    Current->render();
}

void SceneBasic_Uniform::ShowTransformWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoResize;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;
    //OpenFileTags |= ImGuiWindowFlags_NoCollapse;

    {
        ImGui::Begin("Transform Object", &show_transform, OpenFileTags);
        ImGui::Text("Transform Object");
        ImGui::SliderFloat("Left - Right", &ObjX, -10.0f, 10.0f);
        ImGui::SameLine(); Help("X Axis");
        ImGui::NewLine();

        ImGui::SliderFloat("Down - Up", &ObjY, -10.0f, 10.0f);
        ImGui::SameLine(); Help("Y Axis");
        ImGui::NewLine();

        ImGui::SliderFloat("Back - Front", &ObjZ, -10.0f, 10.0f);
        ImGui::SameLine(); Help("Z Axis");
        ImGui::NewLine();

        ImGui::End();
    }
}

void SceneBasic_Uniform::ShowScaleWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoResize;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;
    //OpenFileTags |= ImGuiWindowFlags_NoCollapse;

    {
        ImGui::Begin("Scale Object", &show_Scale, OpenFileTags);
        ImGui::Text("Scale Object");
        ImGui::SliderFloat("Size", &ObjScale, 0.2f, 10.0f);
        ImGui::SameLine(); Help("Smaller - Bigger");
        ImGui::NewLine();

        ImGui::End();
    }
}

void SceneBasic_Uniform::ShowRotationWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoResize;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;
    //OpenFileTags |= ImGuiWindowFlags_NoCollapse;

    {
        ImGui::Begin("Rotate Object", &show_Rotation, OpenFileTags);
        ImGui::Text("Rotate Object");

        ImGui::Text("X Axis Rotation");
        ImGui::SliderAngle("X Angle", &ObjRotationX);
        ImGui::SameLine(); if (ImGui::Button("Reset X"))
        {
            ObjRotationX = 0.0f;
        }
        ImGui::NewLine();

        ImGui::Text("Y Axis Rotation");
        ImGui::SliderAngle("Y Angle", &ObjRotationY);
        ImGui::SameLine(); if (ImGui::Button("Reset Y"))
        {
            ObjRotationY = 0.0f;
        }
        ImGui::NewLine();

        ImGui::Text("Z Axis Rotation");
        ImGui::SliderAngle("Z Angle", &ObjRotationZ);
        ImGui::SameLine(); if (ImGui::Button("Reset Z"))
        {
            ObjRotationZ = 0.0f;
        }
        ImGui::NewLine();

        ImGui::Text("Auto Rotate Y?");
        ImGui::Checkbox("Click!", &AutoRotate);
        ImGui::SameLine(); //ADD SLIDER FOR SPEED HERE

        ImGui::End();
    }
}

void SceneBasic_Uniform::ShowWireframeWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    //OpenFileTags |= ImGuiWindowFlags_NoResize;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;
    //OpenFileTags |= ImGuiWindowFlags_NoCollapse;

    {
        ImGui::Begin("Wireframe Options", &show_Rotation, OpenFileTags);
        ImGui::Text("Line Settings");
        ImGui::SliderFloat("Line Width", &ObjWFWidth, 0.20f, 2.0f);
        if (ImGui::Button("Reset Width"))
        {
            ObjWFWidth = 0.75f;
        }

        ImGui::NewLine();
        ImGui::ColorEdit3("Line Colour", (float*)&ObjWFColor);
        if (ImGui::Button("Reset Line Colour"))
        {
            ObjWFColor = vec4(0.05, 0.0f, 0.05f, 1.0f);
        }

        ImGui::NewLine();
        ImGui::ColorEdit3("Back Colour", (float*)&ObjWFBack);
        if (ImGui::Button("Reset Back Colour"))
        {
            ObjWFBack = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        ImGui::End();
    }
}

void SceneBasic_Uniform::Help(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


void SceneBasic_Uniform::WireFrame()
{
    Shader_WireFrame.use();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader_WireFrame.setUniform("Line.Width", ObjWFWidth);
    Shader_WireFrame.setUniform("Line.Color", ObjWFColor);
    Shader_WireFrame.setUniform("Line.BackColor", ObjWFBack);

    vec3 cameraPos = vec3(0.0f);
    cameraPos = vec3(0.0f * cos(90), 2.0f, 5.5f * sin(90));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);
    model = glm::translate(model, vec3(ObjX, ObjY, ObjZ));
    model = glm::scale(model, vec3(ObjScale, ObjScale, ObjScale));

    if (!AutoRotate)
    {
        model = glm::rotate(model, glm::radians(ObjRotationX * 57.5f), vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(ObjRotationY * 57.5f), vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(ObjRotationZ * 57.5f), vec3(0.0f, 0.0f, 1.0f));
    }
    else
    {
        ObjRotationX = 0.0f;
        ObjRotationY = 0.0f;
        ObjRotationZ = 0.0f;
        model = glm::rotate(model, glm::radians((float)angle_2 * AutoRotateSpeed), vec3(0.0f, 1.0f, 0.0f));

    }
    setMatrices6();
    Current->render();

    glFinish();
}

void SceneBasic_Uniform::WireFramePre()
{
    Shader_WireFrame.use();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    float c = 1.5f;
    //projection2 = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 1.0f, 100.0f);


    Shader_WireFrame.setUniform("Line.Width", 0.75f);
    Shader_WireFrame.setUniform("Line.Color", vec4(0.05, 0.0f, 0.05f, 1.0f));
    Shader_WireFrame.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    Shader_WireFrame.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Shader_WireFrame.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    Shader_WireFrame.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
    Shader_WireFrame.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    Shader_WireFrame.setUniform("Material.Shininess", 100.0f);
}