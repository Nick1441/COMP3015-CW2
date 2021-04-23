#include "scenebasic_uniform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iostream>
#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_glfw_gl3.h"
//#include "gl3w/gl3w.h"

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


//Setting Components Used Throughout Project
SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100), angle_2(0.0f), angle_1(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), sky(100.0f)
{
    //Loading In Models, Just Texture & Normal Mapping Models.
    CarModel = ObjMesh::load("media/SportsCar.obj", true);
    CarModelNormal = ObjMesh::load("media/SportsCar.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    
    compile();                                                                              //Compile all shaders.
    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0f);                                                                //Default Projection Mat
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
   
    //IMGUI
    ImGui_ImplGlfwGL3_NewFrame();
    

    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        //ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }

   // //int display_w, display_h;
   // //glfwGetFramebufferSize(window, &display_w, &display_h);
   // //glViewport(0, 0, display_w, display_h);
   //// glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
   // //glClear(GL_COLOR_BUFFER_BIT);
   //


    if (OverallLoad == 1)
    {
        Scenario_1();
        RenderSkyBox();
    }
    else if (OverallLoad == 2)
    {
        Scenario_2();
        RenderSkyBox();
    }
    else if (OverallLoad == 3)
    {
        Scenario_3();
        RenderSkyBox();
    }
    else if (OverallLoad == 4)
    {
        Scenario_4();
        RenderSkyBox();
    }
    else if (OverallLoad == 5)
    {
        Scenario_5();
        RenderSkyBox();
    }
    ImGui::Render();
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
    CarModelNormal->render();

    //Uses differnt pass for differnt Texture! Pink & Blue!
    Shader_1.setUniform("Pass", 2);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 1.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    setMatrices();
    CarModelNormal->render();

    //Uses differnt pass for differnt Texture! Green & Pink!
    Shader_1.setUniform("Pass", 3);

    //Setting Models Location and Scale
    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, -0.45f, 2.0f));
    model = glm::scale(model, vec3(0.8f, 0.8f, 0.8f));
    setMatrices();
    CarModelNormal->render();
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