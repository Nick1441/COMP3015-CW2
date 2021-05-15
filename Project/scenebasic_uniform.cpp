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

SceneBasic_Uniform::SceneBasic_Uniform() : plane(20.0f, 20.0f, 2, 2), angle_1(0.0f), sky(100.0f), Current(),
time(0), deltaT(0), torus(0.7f * 0.1f, 0.3f * 0.1f, 20, 20), Mix2_nParticles(100), Mix2_particleLifetime(10.5f), Mix2_drawBuf(1), Mix2_emitterPos(0.0f, -1.0f, 0.0f), Mix2_emitterDir(0, 1, 0)
{
    //Nothing In here, Everything Required inside initScene()
}

//  --- Essential SceneBaic Methods ---
void SceneBasic_Uniform::initScene()
{
    compile();                                              //Compile All Shaders.
  
    glEnable(GL_DEPTH_TEST);                                //Enable Depth Test. Defualt Shader Required.
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    //Loading Defualt Texture/Object, Default is Shadow Silhouette
    Current = ObjMesh::loadWithAdjacency("media/SportsCar.obj");
    CurrentTextureGL = Texture::loadTexture("media/SportsCar1.png");
    SSBackground = Texture::loadTexture("media/texture/brick1.jpg");
    
    GLuint cubeTex = Texture::loadCubeMap("media/texture/cube/Skybox/skybox2"); //Skybox Texture
    GLuint noiseTex = NoiseTex::generate2DTex(6.0f);        //Loading Noise Texture;
    
    //Setting Textures inside Locations. Same ones used in differnt textures.
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, CurrentTextureGL);         //Objects Texture

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, SSBackground);             //Wall Background

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, noiseTex);                 //For clouds

    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);            //For Skybox

    //Setting Default strings for texture locations.
    Mix1_Plane = ObjMesh::loadWithAdjacency("media/Plane.obj");
    ObjLocation = "SportsCar";
    TextureLocation = "SportsCar1";

    //Setting Camera Pos, as this does now not change!
    CameraPos = vec3(0.0f * cos(90), 2.0f, 5.5f * sin(90));

    //Setting some styling for ImGUI. Looks Better!
    ImGui::GetStyle().WindowRounding = 6.0f;
    ImGui::GetStyle().GrabRounding = 5.0f;
    ImGui::GetStyle().FrameRounding = 4.0f;

    //initalising some Shaders. Some do not require.
    Mix2_init();
    Wireframe_init();
    Silhouette_init();

    Cloud_init();
    Wood_init();
    Mix1_init();

    EdgeDetection_init();
    ParticleEffects_init();
    CustomParticle_init();
}
void SceneBasic_Uniform::compile()
{
    //Loading/Linking Shader Programs! There is alot... & any feedback handlers!
    try {
        //     ------ Shadow Volumes + Silhouette ------
        //Creating Mix1 Shaders
        Shader_Mix1_Volume.compileShader("shader/ShadowSilhouette/Volume.vert");
        Shader_Mix1_Volume.compileShader("shader/ShadowSilhouette/Volume.frag");
        Shader_Mix1_Volume.compileShader("shader/ShadowSilhouette/Volume.geom");
        Shader_Mix1_Volume.link();

        Shader_Mix1_Comp.compileShader("shader/ShadowSilhouette/Comp.vert");
        Shader_Mix1_Comp.compileShader("shader/ShadowSilhouette/Comp.frag");
        Shader_Mix1_Comp.link();

        Shader_Mix1_Render.compileShader("shader/ShadowSilhouette/Render.vert");
        Shader_Mix1_Render.compileShader("shader/ShadowSilhouette/Render.frag");
        Shader_Mix1_Render.compileShader("shader/ShadowSilhouette/Render.geom");
        Shader_Mix1_Render.link();

        //     ------ Shader Edges + Particle Effects ------
        //Creating Mix 2 Shader!
        Shader_Mix2.compileShader("shader/ShadowPart/ShadowPart.vert");
        Shader_Mix2.compileShader("shader/ShadowPart/ShadowPart.frag");

        GLuint progHandle = Shader_Mix2.getHandle();
        const char* outputNames[] = { "pPosition", "pVelocity", "pAge", "pRotation" };
        glTransformFeedbackVaryings(progHandle, 4, outputNames, GL_SEPARATE_ATTRIBS);

        Shader_Mix2.link();
        Shader_Mix2.use();

        //     ------ WireFrame ------
        Shader_WireFrame.compileShader("shader/Wireframe/Wireframe.vert");
        Shader_WireFrame.compileShader("shader/Wireframe/Wireframe.frag");
        Shader_WireFrame.compileShader("shader/Wireframe/Wireframe.geom");
        Shader_WireFrame.link();

        //     ------ Silhouette ------
        Shader_Silhouete.compileShader("shader/Silhouette/Silhouette.vert");
        Shader_Silhouete.compileShader("shader/Silhouette/Silhouette.frag");
        Shader_Silhouete.compileShader("shader/Silhouette/Silhouette.geom");
        Shader_Silhouete.link();


        //     ------ Clouds Noise ------
        Shader_Clouds.compileShader("shader/Clouds/Clouds.vert");
        Shader_Clouds.compileShader("shader/Clouds/Clouds.frag");
        Shader_Clouds.link();

        //     ------ Wood Effect Noise ------
        Shader_Wood.compileShader("shader/Wood/Wood.vert");
        Shader_Wood.compileShader("shader/Wood/Wood.frag");
        Shader_Wood.link();

        //     ------ Night Vision Noise ------
        Shader_NightVision.compileShader("shader/NightVision/NightVision.vert");
        Shader_NightVision.compileShader("shader/NightVision/NightVision.frag");
        Shader_NightVision.link();

        Shader_Gaussin.compileShader("shader/Gaussin/Gaussin.vert");
        Shader_Gaussin.compileShader("shader/Gaussin/Gaussin.frag");
        Shader_Gaussin.link();

        //     ------ Edge Detection ------
        Shader_EdgeDetection.compileShader("shader/EdgeDetection/EdgeDetection.vert");
        Shader_EdgeDetection.compileShader("shader/EdgeDetection/EdgeDetection.frag");
        Shader_EdgeDetection.link();

        //     ------ Smoke Particle ------
        Shader_SmokEffect.compileShader("shader/SmokeEffect/SmokeEffect.vert");
        Shader_SmokEffect.compileShader("shader/SmokeEffect/SmokeEffect.frag");

        GLuint progHandle2 = Shader_SmokEffect.getHandle();
        const char* outputNames2[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle2, 3, outputNames2, GL_SEPARATE_ATTRIBS);

        Shader_SmokEffect.link();

        //     ------ Custom Particle Effects ------
        Shader_CustomParticle.compileShader("shader/CustomP/CustomP.vert");
        Shader_CustomParticle.compileShader("shader/CustomP/CustomP.frag");

        GLuint progHandle3 = Shader_CustomParticle.getHandle();
        const char* outputNames3[] = { "Position", "Velocity", "Age", "Rotation" };
        glTransformFeedbackVaryings(progHandle3, 4, outputNames3, GL_SEPARATE_ATTRIBS);

        Shader_CustomParticle.link();

        Shader_SkyBox.compileShader("shader/SkyBox.vert");
        Shader_SkyBox.compileShader("shader/SkyBox.frag");
        Shader_SkyBox.link();
    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
void SceneBasic_Uniform::update(float t)
{
    //Used for Object Rotation throughout
    if (m_animate) {
        angle_1 += 0.05f;
        if (angle_1 >= 360.0f)
            angle_1 -= 360.0f;
    }

    //Used Inside Particle Systems!
    deltaT = t - time;
    time = t;

    //Light Rotation inside Scenes
    LRF += 0.2f * deltaT;
    if (LRF > glm::two_pi<float>()) LRF -= glm::two_pi<float>();
}
void SceneBasic_Uniform::render()
{
    //Setting Light Pos as can be updated is auto Rotate is on!
    lightPos = vec4(5.0f * vec3(cosf(LightAngle) * 7.5f, 1.5f, sinf(LightAngle) * 7.5f), 1.0f);
    
    //Auto Rotate used Light Angle for pos Changes.
    if (Light_AutoRotate)
    {
        LightAngle = LRF;
    }
    
    //Setting Vector for Objects Transform Position.
    ObjTransformVec = vec3(ObjTransform[0], ObjTransform[1], ObjTransform[2]);

    //Clear Buffer But before Each Render.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //GUI Features. Runs Menu + Checks For GUI Window Updates.
    GUIMenuRender();
    GUIWindowRender();

    //Shows Each Render. Each Shader has Own Render.
    if (show_Wireframe)             { Wireframe_render(); }
    else if (show_Silhouette)       { Silhouette_render(); }
    else if (show_Mix1)             { Mix1_render(); }
    else if (show_NightVision)      { NightVision_render(); }
    else if (show_Gaussin)          { Gaussin_render(); }
    else if (show_edgeDetection)    { EdgeDetection_render(); }
    else if (show_Mix2)             { Mix2_render(); }
    else if (show_customParticle)   { CustomParticle_render(); }
    else {  //Incase theyre Smart and Close them all, Defaults To Mix1
        show_Mix1 = true;
        LoadObjectAdj();
    }

    //Render Backgrounds. Not Dependent on Shaders
    if (show_Clouds)                { Cloud_render(); }
    if (show_Wood)                  { Wood_render(); }
    if (show_skyBox)                { RenderSkyBox(); }
    if (show_particleEffects)       { ParticleEffects_render(); }
    if (show_LightPos) { ShowLightPosWindow(); };

    //Imgui Render Commands. Required to Work.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void SceneBasic_Uniform::setMatrices(GLSLProgram &prog)
{
    mat4 mv = view * model;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("ProjMatrix", projection);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    
    prog.setUniform("ShadowMatrix", Mix2_lightPV * model);
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ViewportMatrix", viewport);
}
void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    viewport = mat4(vec4(w2, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, h2, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
}

//            --- Mix 1 ---
//  --- Shadow Volumes + Silhouette ---
void SceneBasic_Uniform::Mix1_init()
{
    //Clearing Stencil, Enabling Depth Test.
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearStencil(0);
    glEnable(GL_DEPTH_TEST);

    //Setting Up FBO's
    Mix1_setUpFBO();

    //Using Shader and Setting Lights Intensity
    Shader_Mix1_Render.use();
    Shader_Mix1_Render.setUniform("LightIntensity", vec3(1.0f));

    //Creating Full Screen VOA
    GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
      1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };

    //Creating Buffer Handle
    GLuint bufHandle;
    glGenBuffers(1, &bufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    // Setting Vertex Arrat Object for Mix1
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    //Setting Vertex Position
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); 

    glBindVertexArray(0);

    //Setting Texture of Current Object.
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, CurrentTextureGL);

    //Setting Inital Values for Shader. Edge + PCT also 
    Shader_Mix1_Render.use();
    Shader_Mix1_Render.setUniform("Tex", 8);
    Shader_Mix1_Render.setUniform("Tex2", 9);

    //Setting Inital Values for Diffspec In shader.
    Shader_Mix1_Comp.use();
    Shader_Mix1_Comp.setUniform("DiffSpecTex", 4);

    //using Volume, as Required First.
    Shader_Mix1_Volume.use();
}
void SceneBasic_Uniform::Mix1_render()
{
    // Renders Each Pass, with flushing inbetween.
    Mix1_pass1();
    glFlush();
    Mix1_pass2();
    glFlush();
    Mix1_pass3();
}
void SceneBasic_Uniform::Mix1_setUpFBO()
{
    //Depth Buffer Creation.
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    //Ambient Buffer Creation.
    GLuint ambBuf;
    glGenRenderbuffers(1, &ambBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, ambBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

    //Diffuse/Specular Creation. Setting it to Texture binding 4.
    glActiveTexture(GL_TEXTURE4);
    GLuint diffSpecTex;
    glGenTextures(1, &diffSpecTex);
    glBindTexture(GL_TEXTURE_2D, diffSpecTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //FBO Setup
    glGenFramebuffers(1, &colorDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffSpecTex, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    //This si a little Saftey Bit the examples had, Usefull incase it fails!
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    }
    else {
        printf("Framebuffer is not complete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SceneBasic_Uniform::Mix1_pass1()
{
    //This path Renders Scene Normaly, But writes Shaded Colour to the Two Buffers!
    //Enable Requirements for Shadows to work.
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    projection = glm::infinitePerspective(glm::radians(50.0f), (float)width / height, 0.5f);
    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Resetting lightpos as User Can change this!
    Shader_Mix1_Render.use();
    Shader_Mix1_Render.setUniform("LightPosition", view * lightPos);

    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    //Drawing Scene as normal!
    Mix1_drawscene(Shader_Mix1_Render, false);
}
void SceneBasic_Uniform::Mix1_pass2()
{
    //This pass Sets up Stencil Buffer, Front faces cause Increment, Back causes Decrement.
    //Only shadows are passed from geom to frag!

    //Setting Light Pos so its same for all shaders!
    Shader_Mix1_Volume.use();
    Shader_Mix1_Volume.setUniform("LightPosition", view * lightPos);

    //This stores the Ambient Component
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorDepthFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);

    //Disable writing to this buffer.
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    //Rebind to default Frame Buffer!
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //This is where is Increments or Decrements.
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xffff);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

    //Drawing Scene as normal!
    Mix1_drawscene(Shader_Mix1_Volume, true);

    //Enable writingg to colour buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
void SceneBasic_Uniform::Mix1_pass3()
{
    //Combines data from each pass when the Stencil test Sucseeds.

    //Not Required for this pass.
    glDisable(GL_DEPTH_TEST);

    //Enables Blend Fuction
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    //Renders only pixels that have stencil value of 0
    glStencilFunc(GL_EQUAL, 0, 0xffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    Shader_Mix1_Comp.use();

    //Draw a Full screen Quad.
    model = mat4(1.0f);
    projection = model;
    view = model;
    setMatrices(Shader_Mix1_Comp);

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    //Restore and Enable for Next Render.
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
void SceneBasic_Uniform::Mix1_drawscene(GLSLProgram& prog, bool onlyShadowCasters)
{
    //Only Renders on pass1, as they dont cast shadows.
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, CurrentTextureGL);
        prog.setUniform("Ka", Mix1Color * 0.1f);
        prog.setUniform("Kd", Mix1Color);
        prog.setUniform("Ks", vec3(0.9f));
        prog.setUniform("Shininess", 150.0f);

        //Reseting Values, this can be updated Live through GUI!
        prog.setUniform("EdgeWidth", Mix1EW);
        prog.setUniform("PctExtend", Mix1PE);
        prog.setUniform("Type", 1);

        //Can have toon Shading or BlinnPhong with Texturing!
        if (toonShadingActive)
        {
            ToonOrNot = 1;
        }
        else
        {
            ToonOrNot = 0;
        }

        prog.setUniform("RenderType", ToonOrNot);
    }

    //Resets Model
    model = mat4(1.0f);
    model = glm::translate(model, ObjTransformVec);             //GUI can change Pos.
    model = glm::scale(model, vec3(ObjScale, ObjScale, ObjScale));  //GUI can change Rotation.

    //Setting Rotation of object. This is either "Auto" or manualy set angles.
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
        model = glm::rotate(model, glm::radians((float)angle_1 * AutoRotateSpeed), vec3(0.0f, 1.0f, 0.0f));

    }
    model = glm::scale(model, vec3(ObjScale));          //Setting Scale of Object.
    setMatrices(prog);
    Current->render();                                  //Render Current Object, This can be changed through GUI

    //Only if shadow casters is off, set all info down as do not want as strong light or shinnines.
    if (!onlyShadowCasters) {

        //Reset Texture into Slot 9
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, SSBackground);
        prog.setUniform("Kd", Mix1Color * vec3(0.1f));
        prog.setUniform("Ks", vec3(0.0f));
        prog.setUniform("Ka", vec3(0.1f));
        prog.setUniform("Shininess", 1.0f);
        prog.setUniform("EdgeWidth", 0.0f);
        prog.setUniform("PctExtend", 0.0f);
        prog.setUniform("levels", Mix1Levels);
        prog.setUniform("Type", 0);

        //Render Walls Objects.
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, -0.169f, 0.0f));
        setMatrices(prog);
        Mix1_Plane->render();
        model = mat4(1.0f);
        model = glm::translate(model, vec3(-3.25f, 5.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
        setMatrices(prog);
        Mix1_Plane->render();
        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices(prog);
        Mix1_Plane->render();
        model = mat4(1.0f);
    }
}

//            --- Mix 2 ---
//  --- Shadow Soft Edges + Particle Systyem ---
void SceneBasic_Uniform::Mix2_init()
{
    //Setting Enables/Disables, Swapping from all shaders, can still run.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);

    //Reseting Proj/View as other shaders can change this.
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);
    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting Initial Values. Could be done in constructor but OCD.
    Mix2_shadowMapWidth = 1024;
    Mix2_shadowMapHeight = 1024;

    Mix2_samplesU = 4;
    Mix2_samplesV = 8;
    Mix2_jitterMapSize = 8;
    Mix2_radius = 7.0f;

    //Creating the Texture from particles amount.
    glActiveTexture(GL_TEXTURE2);
    ParticleUtils::createRandomTex1D(Mix2_nParticles * 4);

    //Setting Up FBO's & building Jitter!
    Mix2_setUpFBO();
    Mix2_initBuffers();
    Mix2_buildJitter();

    //Creating Feedback handle.
    GLuint programHandle = Shader_Mix2.getHandle();
    Mix2_pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
    Mix2_pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shaderWithShadows");

    //Shadow Scale for ...well... Shadow...
    Mix2_shadowScale = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.5f, 0.5f, 0.5f, 1.0f));

    //Setting Lightpos as this is differnt to all other shaders. Not Updatable..
    lightpos = vec3(2.5f, 6.0, 2.5f);
    lightFrustum.orient(lightpos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    lightFrustum.setPerspective(40.0f, 1.0f, 1.0f, 100.0f);
    Mix2_lightPV = Mix2_shadowScale * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

    //Setting Uniforms. These can be changed, Inwhich this is recalled!
    Shader_Mix2.setUniform("Light.Intensity", vec3(0.85f));
    Shader_Mix2.setUniform("ShadowMap", 0);
    Shader_Mix2.setUniform("OffsetTex", 1);
    Shader_Mix2.setUniform("Radius", Mix2_radius / 512.0f);
    Shader_Mix2.setUniform("OffsetTexSize", vec3(Mix2_jitterMapSize, Mix2_jitterMapSize, Mix2_samplesU * Mix2_samplesV / 2.0f));
    Shader_Mix2.setUniform("RandomTex", 2);
    Shader_Mix2.setUniform("Emitter", Mix2_emitterPos);
    Shader_Mix2.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(Mix2_emitterDir));
}
void SceneBasic_Uniform::Mix2_render()
{
    //Set changed to work in real time from GUI!
    Shader_Mix2.use();
    Shader_Mix2.setUniform("ParticleLifetime", Mix2_particleLifetime);
    Shader_Mix2.setUniform("Accel", vec3(0.0f, -Mix2AccelAmount, 0.0));

    //Generate Shadow Map
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, Mix2_shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, Mix2_shadowMapWidth, Mix2_shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &Mix2_pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);

    //Render everything which will produce shadow map.
    Mix2_draw();
    glDisable(GL_POLYGON_OFFSET_FILL);

    //Render Everything Normally
    vec3 cameraPos(5.0f * cos(0), 4.7f, 5.0f * sin(0));
    view = glm::lookAt(cameraPos, vec3(0.0f, -0.175f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    Shader_Mix2.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0));
    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &Mix2_pass2Index);
    glDisable(GL_CULL_FACE);

    //Draw scene again, this time, light etc will be used.
    Mix2_draw();

    glFinish();
}
void SceneBasic_Uniform::Mix2_draw()
{
    //Redner Particle System
    Shader_Mix2.setUniform("Type", 0);  
    Mix2_particleRender(Shader_Mix2);                                   //Particle System rendering in antoher method to slit this up.

    //Render Plane Under Particle System
    Shader_Mix2.setUniform("Type", 1);

    Shader_Mix2.setUniform("Material.Kd", vec3(0.1f) * Mix2LightCol);   //Update Colour info, weeker Kd than object.
    Shader_Mix2.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    Shader_Mix2.setUniform("Material.Ka", 0.05f, 0.05f, 0.05f);
    Shader_Mix2.setUniform("Material.Shininess", 1.0f);
    model = mat4(1.0f);
    setMatrices(Shader_Mix2);
    plane.render();
}
void SceneBasic_Uniform::Mix2_setUpFBO()
{
    GLfloat border[] = { 1.0f, 0.0f,0.0f,0.0f };

    //Create Deph Buffer
    GLuint depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, Mix2_shadowMapWidth, Mix2_shadowMapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    //Bind Depth Buffer to Texture 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    //Create & Setup FBO
    glGenFramebuffers(1, &Mix2_shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, Mix2_shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    //Addon used in lectures. Useful to make sure everything is running.
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    }
    else {
        printf("Framebuffer is not complete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SceneBasic_Uniform::Mix2_buildJitter()
{
    //Creating Jitter Map for Shadows
    int size = Mix2_jitterMapSize;
    int samples = Mix2_samplesU * Mix2_samplesV;
    int bufSize = size * size * samples * 2;
    float* data = new float[bufSize];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < samples; k += 2) {
                int x1, y1, x2, y2;
                x1 = k % (Mix2_samplesU);
                y1 = (samples - 1 - k) / Mix2_samplesU;
                x2 = (k + 1) % Mix2_samplesU;
                y2 = (samples - 1 - k - 1) / Mix2_samplesU;

                vec4 v;
                //Center on grid and Mix2_jitterCalc
                v.x = (x1 + 0.5f) + Mix2_jitterCalc();
                v.y = (y1 + 0.5f) + Mix2_jitterCalc();
                v.z = (x2 + 0.5f) + Mix2_jitterCalc();
                v.w = (y2 + 0.5f) + Mix2_jitterCalc();

                //Scale between 0 and 1
                v.x /= Mix2_samplesU;
                v.y /= Mix2_samplesV;
                v.z /= Mix2_samplesU;
                v.w /= Mix2_samplesV;

                //Warp to disk
                int cell = ((k / 2) * size * size + j * size + i) * 4;
                data[cell + 0] = sqrtf(v.y) * cosf(glm::two_pi<float>() * v.x);
                data[cell + 1] = sqrtf(v.y) * sinf(glm::two_pi<float>() * v.x);
                data[cell + 2] = sqrtf(v.w) * cosf(glm::two_pi<float>() * v.z);
                data[cell + 3] = sqrtf(v.w) * sinf(glm::two_pi<float>() * v.z);
            }
        }
    }

    //Bind to texture 1
    glActiveTexture(GL_TEXTURE1);
    GLuint texID;
    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_3D, texID);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, size, size, samples / 2);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, size, size, samples / 2, GL_RGBA, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //Delete what is un required.
    delete[] data;
}
float SceneBasic_Uniform::Mix2_jitterCalc()
{
    //Returns random float between -0.5 & 0.5
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distrib(-0.5f, 0.5f);
    return distrib(generator);
}
void SceneBasic_Uniform::Mix2_initBuffers()
{
    //Creating Feedback buffers for Particle System
    glGenBuffers(2, Mix2_posBuf);
    glGenBuffers(2, Mix2_velBuf);
    glGenBuffers(2, Mix2_age);
    glGenBuffers(2, Mix2_rotation);

    //Allocating space for each buffer
    int size = Mix2_nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_age[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_age[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_rotation[0]);
    glBufferData(GL_ARRAY_BUFFER, 2 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_rotation[1]);
    glBufferData(GL_ARRAY_BUFFER, 2 * size, 0, GL_DYNAMIC_COPY);

    //Creating and filling first age buffer
    std::vector<GLfloat> initialAges(Mix2_nParticles);
    float rate = Mix2_particleLifetime / Mix2_nParticles;

    for (int i = 0; i < Mix2_nParticles; i++)
    {
        initialAges[i] = rate * (i - Mix2_nParticles);
    }

    //Creating vertex arrays for each set of buffers.
    glBindBuffer(GL_ARRAY_BUFFER, Mix2_age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Mix2_nParticles * sizeof(float), initialAges.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(2, Mix2_particleArray);

    glBindVertexArray(Mix2_particleArray[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus.getElementBuffer());

    glBindBuffer(GL_ARRAY_BUFFER, torus.getPositionBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, torus.getNormalBuffer());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_posBuf[0]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_velBuf[0]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_age[0]);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_rotation[0]);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(6);


    glBindVertexArray(Mix2_particleArray[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus.getElementBuffer());

    glBindBuffer(GL_ARRAY_BUFFER, torus.getPositionBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, torus.getNormalBuffer());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_posBuf[1]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_velBuf[1]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_age[1]);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, Mix2_rotation[1]);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);

    //Creating Feedback objects
    glGenTransformFeedbacks(2, Mix2_feedback);

    //Feedback in slot [0]
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, Mix2_feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, Mix2_posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, Mix2_velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, Mix2_age[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, Mix2_rotation[0]);

    //Feedback in slot [1]
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, Mix2_feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, Mix2_posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, Mix2_velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, Mix2_age[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, Mix2_rotation[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}
void SceneBasic_Uniform::Mix2_particleRender(GLSLProgram& prog)
{
    //Set time etc so can calculate where each particle should be.
    prog.use();
    prog.setUniform("Time", time);
    prog.setUniform("DeltaT", deltaT);
    prog.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, Mix2_feedback[Mix2_drawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(Mix2_particleArray[1 - Mix2_drawBuf]);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glVertexAttribDivisor(3, 0);
    glVertexAttribDivisor(4, 0);
    glVertexAttribDivisor(5, 0);
    glVertexAttribDivisor(6, 0);

    glDrawArrays(GL_POINTS, 0, Mix2_nParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    prog.setUniform("Pass", 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set cam/view for render.
    vec3 cameraPos(5.0f * cos(0), 4.7f, 5.0f * sin(0));
    view = glm::lookAt(cameraPos, vec3(0.0f, -0.175f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    prog.setUniform("Material.Kd", Mix2LightCol);
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Ka", Mix2LightCol * vec3(0.1f));
    prog.setUniform("Material.E", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 1.0f, 0.0f));
    setMatrices(Shader_Mix2);

    glBindVertexArray(Mix2_particleArray[Mix2_drawBuf]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glDrawElementsInstanced(GL_TRIANGLES, torus.getNumVerts(), GL_UNSIGNED_INT, 0, Mix2_nParticles);

    prog.setUniform("Material.Kd", 0.0f, 0.0, 0.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Ka", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.E", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 1.0f);

    //Finaly swap buffers
    Mix2_drawBuf = 1 - Mix2_drawBuf;
}


//  --- Wireframe Infomation ---
void SceneBasic_Uniform::Wireframe_init()
{
    //Setting Clear Colour and Using Correct Shader.
    Shader_WireFrame.use();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    //Setting initial Values for Uniforms.
    Shader_WireFrame.setUniform("Line.Width", 0.75f);
    Shader_WireFrame.setUniform("Line.Color", vec4(0.05, 0.0f, 0.05f, 1.0f));
    Shader_WireFrame.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    Shader_WireFrame.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Shader_WireFrame.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    Shader_WireFrame.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
    Shader_WireFrame.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    Shader_WireFrame.setUniform("Material.Shininess", 100.0f);
}
void SceneBasic_Uniform::Wireframe_render()
{
    Shader_WireFrame.use();

    //These will get updated Using ImGUI
    Shader_WireFrame.setUniform("Line.Width", ObjWFWidth);
    Shader_WireFrame.setUniform("Line.Color", ObjWFColor);
    Shader_WireFrame.setUniform("Line.BackColor", ObjWFBack);

    //Calls General Render. This is used multiple Times to save repeted code.
    GeneralRender(Shader_WireFrame);
}


//  --- Silhouette Infomation ---
void SceneBasic_Uniform::Silhouette_init()
{
    Shader_Silhouete.use();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    //Inital Values For Silhoutte Shader. These will Not Change.
    Shader_Silhouete.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    Shader_Silhouete.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
}
void SceneBasic_Uniform::Silhouette_render()
{
    //Changing Uniforms as can be changed through ImGUI
    Shader_Silhouete.use();
    Shader_Silhouete.setUniform("EdgeWidth", Sil_EdgeWidth);
    Shader_Silhouete.setUniform("PctExtend", Sil_PctExtend);
    Shader_Silhouete.setUniform("LineColor", Sil_LineColor);
    Shader_Silhouete.setUniform("Light.Position", lightPos);
    Shader_Silhouete.setUniform("Material.Kd", Sil_LightColor);
    Shader_Silhouete.setUniform("levels", Sil_Levels);

    //Calls General Render. This is used multiple Times to save repeted code.
    GeneralRender(Shader_Silhouete);
}


//  --- Clouds Background ---
void SceneBasic_Uniform::Cloud_init()
{
    //Creating Full Screen VAO
    Shader_Clouds.use();
    GLfloat verts[] = {
       -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    };

    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    //Creating Buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //Setting Noise Text Binding to 10, as this is set at start.
    Shader_Clouds.setUniform("NoiseTex", 10);
}
void SceneBasic_Uniform::Cloud_render()
{
    Shader_Clouds.use();

    //Variables that can be changed through ImGUI
    Shader_Clouds.setUniform("SkyColor", Clouds_Sky);
    Shader_Clouds.setUniform("CloudColor", Clouds_Cloud);

    //Positions Quad so it is at the back, behind Object.
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0, -0.2f, -5.0f));
    model = glm::rotate(model, glm::radians((float)-25), vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, vec3(12.0));
    setMatrices(Shader_Clouds);

    //Draw the Quad with clouds on.
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glFinish();
}


//  --- Wood Grain Background ---
void SceneBasic_Uniform::Wood_init()
{
    //Creating Full Screen VAO
    Shader_Wood.use();
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    };

    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    //Create Buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    Shader_Wood.setUniform("NoiseTex", 3);

    //Created Slices to generate pattern. Could be sliced more for differnt looks.
    mat4 slice;
    slice = glm::rotate(slice, glm::radians(10.0f), vec3(1.0, 0.0, 0.0));
    slice = glm::rotate(slice, glm::radians(-20.0f), vec3(0.0, 0.0, 1.0));
    slice = glm::scale(slice, vec3(40.0, 40.0, 1.0));
    slice = glm::translate(slice, vec3(-0.35, -0.5, 2.0));

    Shader_Wood.setUniform("Slice", slice);

    //Bind Texture.
    GLuint noiseTex = NoiseTex::generate2DTex();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
}
void SceneBasic_Uniform::Wood_render()
{
    Shader_Wood.use();

    //Setting Uniforms which change due to ImGUI input.
    Shader_Wood.setUniform("DarkWoodColor", DarkWoodColor);
    Shader_Wood.setUniform("LightWoodColor", LightWoodColor);

    //Set Position so it is behind Objects.
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0, -0.2f, -5.0f));
    model = glm::rotate(model, glm::radians((float)-25), vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, vec3(12.0));
    setMatrices(Shader_Wood);

    //Render Quad.
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glFinish();
}


//  --- Loading Textures/Objects For Differnt Shaders ---
void SceneBasic_Uniform::LoadObject()
{
    if (AdjLoaded || NewFileLoad) //Bool Checks to see if its already loaded, saving it re loading, saving time
    {
        //Loading New Object Into Current
        string NewObj = "media/" + ObjLocation + ".obj";
        Current = ObjMesh::load(NewObj.c_str(), true);

        //Loading New Texture
        string NewTexture = "media/" + TextureLocation + ".png";
        CurrentTextureGL = Texture::loadTexture(NewTexture.c_str());  //This is then set to Binding which is used throughout out this texture.

        //Then loaded into slot 1, where all other shaders use this which require Obj Texture.
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, CurrentTextureGL);
        AdjLoaded = false;
        NewFileLoad = false;
    }
}
void SceneBasic_Uniform::LoadObjectAdj()
{
    if (!AdjLoaded || NewFileLoad)
    {
        //Loading New Object Into Current
        string NewObj = "media/" + ObjLocation + ".obj";
        Current = ObjMesh::loadWithAdjacency(NewObj.c_str());

        //Loading New Texture
        string NewTexture = "media/" + TextureLocation + ".png";
        CurrentTextureGL = Texture::loadTexture(NewTexture.c_str());  //This is then set to Binding which is used throughout out this texture.

        //Then loaded into slot 1, where all other shaders use this which require Obj Texture.
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, CurrentTextureGL);
        AdjLoaded = true;
        NewFileLoad = false;
    }
}


//  --- Night Vision Shader ---
void SceneBasic_Uniform::NightVision_init()
{
    Shader_NightVision.use();
    NightVision_setUpFBO();

    //Array to create a full screen quad.
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    //Buffer Setup
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    //Set up Vertex Array Objects.
    glGenVertexArrays(1, &NVQuad);
    glBindVertexArray(NVQuad);

    //Set Up Vertex Position
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    //Set Texture Coords.
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //Creating subroutine indexes
    GLuint programHandle = Shader_NightVision.getHandle();
    NVpass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass1");
    NVpass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass2");

    //Setting Uniforms.
    Shader_NightVision.setUniform("Width", width);
    Shader_NightVision.setUniform("Height", height);
    Shader_NightVision.setUniform("Radius", width / 3.5f);
    Shader_NightVision.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));

    //Binding Texture to 5
    NVnoiseTex = NoiseTex::generatePeriodic2DTex(200.0f, 0.5f, 512, 512);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, NVnoiseTex);

    Shader_NightVision.setUniform("RenderTex", 6);
    Shader_NightVision.setUniform("NoiseTex", 5);
}
void SceneBasic_Uniform::NightVision_render()
{
    //Completes each pass, with a flush inbetween
    Shader_NightVision.use();
    NightVision_pass1();
    glFlush();
    NightVision_pass2();
}
void SceneBasic_Uniform::NightVision_setUpFBO()
{
    glGenFramebuffers(1, &NVrenderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, NVrenderFBO);

    //Create Texture Object.
    glGenTextures(6, &NVrenderTex);
    glBindTexture(GL_TEXTURE_2D, NVrenderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Bind Texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NVrenderTex, 0);

    //Creating Depth Buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    //Depth Buffer binding to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);

    //Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    //Unbind and revert frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SceneBasic_Uniform::NightVision_pass1()
{
    //Create object and render.
    glBindFramebuffer(GL_FRAMEBUFFER, NVrenderFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &NVpass1Index);

    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    //Set Unifroms
    Shader_NightVision.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Shader_NightVision.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
    Shader_NightVision.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    Shader_NightVision.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    Shader_NightVision.setUniform("Material.Shininess", 100.0f);

    GeneralRender(Shader_NightVision);
}
void SceneBasic_Uniform::NightVision_pass2()
{
    //Create Quad, with Night vision colour/noise, and a black outside also!
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, NVrenderTex);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &NVpass2Index);
    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices(Shader_NightVision);

    //Render Quad
    glBindVertexArray(NVQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


//  --- Guassin Blur Shader ---
void SceneBasic_Uniform::Gaussin_init()
{
    Shader_Gaussin.use();
    Gaussin_setUpFBO();

    //Setting up full screen Quad
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, CurrentTextureGL);

    //Creating Buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &GQuad);
    glBindVertexArray(GQuad);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //Set Uniforms
    Shader_Gaussin.setUniform("Light.L", vec3(1.0f));
    Shader_Gaussin.setUniform("Light.La", vec3(0.2f));

    float weights[5], sum, sigma2 = GaussingBlurAmount; // THIS IS WHAT WE SET AS CHANGING

    weights[0] = Gaussin_calcG(0, sigma2);
    sum = weights[0];
    for (int i = 1; i < 5; i++) {
        weights[i] = Gaussin_calcG(float(i), sigma2);
        sum += 2 * weights[i];
    }
    //Normalize the weights and set the uniform
    for (int i = 0; i < 5; i++) {
        std::stringstream uniName;
        uniName << "Weight[" << i << "]";
        float val = weights[i] / sum;
        Shader_Gaussin.setUniform(uniName.str().c_str(), val);


    }
}
void SceneBasic_Uniform::Gaussin_render()
{
    //Run Each Pass. Make sure shader is also being used.
    Shader_Gaussin.use();
    Gaussin_pass1();
    Gaussin_pass2();
    Gaussin_pass3();
}
void SceneBasic_Uniform::Gaussin_pass1()
{
    Shader_Gaussin.setUniform("Pass", 1);

    //Binding Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, GrenderFBO);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    //Setting Uniforms
    Shader_Gaussin.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Shader_Gaussin.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
    Shader_Gaussin.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    Shader_Gaussin.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    Shader_Gaussin.setUniform("Material.Shininess", 100.0f);

    //Using General Render to prevent Code Duplication.
    GeneralRender(Shader_Gaussin);
}
void SceneBasic_Uniform::Gaussin_pass2()
{
    Shader_Gaussin.setUniform("Pass", 2);

    //Binding Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, GIntermediateFBO);

    //Binding Texture
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, GrenderTex);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices(Shader_Gaussin);

    //Drawing Quad to Screen.
    glBindVertexArray(GQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void SceneBasic_Uniform::Gaussin_pass3()
{
    Shader_Gaussin.setUniform("Pass", 3);

    //Binding Frame Buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, GIntermediateTex);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices(Shader_Gaussin);

    //Redraw Quad to screen
    glBindVertexArray(GQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void SceneBasic_Uniform::Gaussin_setUpFBO()
{
    //Creating/Setup of FBO
    glGenFramebuffers(1, &GrenderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, GrenderFBO);

    glGenTextures(1, &GrenderTex);
    glBindTexture(GL_TEXTURE_2D, GrenderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GrenderTex, 0);

    //Creating Depth Buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    //Creating Draw Buffers
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &GIntermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, GIntermediateFBO);

    glGenTextures(1, &GIntermediateTex);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, GIntermediateTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GIntermediateTex, 0);

    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
float SceneBasic_Uniform::Gaussin_calcG(float x, float sigma2)
{
    double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
    double expon = -(x * x) / (2.0 * sigma2);
    return (float)(coeff * exp(expon));
}


//  --- Edge Detection ---
void SceneBasic_Uniform::EdgeDetection_init()
{
    //Setting items back to default as some shaders change these.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);
    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting up FBO's
    EdgeDetection_setUpFBO();

    //Creating Full screen VAO
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    //Creating Handles
    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &EDquad);
    glBindVertexArray(EDquad);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //Setting Unifroms.
    Shader_EdgeDetection.setUniform("EdgeThreshold", 0.05f);
    Shader_EdgeDetection.setUniform("Light.L", vec3(1.0f));
    Shader_EdgeDetection.setUniform("Light.La", vec3(0.2f));
}
void SceneBasic_Uniform::EdgeDetection_render()
{
    Shader_EdgeDetection.use();
    EdgeDetection_pass1();
    glFlush();
    EdgeDetection_pass2();
}
void SceneBasic_Uniform::EdgeDetection_pass1()
{
    Shader_EdgeDetection.setUniform("Pass", 1);
    glBindFramebuffer(GL_FRAMEBUFFER, EDfboHandle);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Setting these so it is same as any other scene
    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);

    //Setting Uniforms
    Shader_EdgeDetection.setUniform("Light.Position", lightPos);
    Shader_EdgeDetection.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
    Shader_EdgeDetection.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    Shader_EdgeDetection.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    Shader_EdgeDetection.setUniform("Material.Shininess", 100.0f);

    //Uses general Render to prevent duplication.
    GeneralRender(Shader_EdgeDetection);
}
void SceneBasic_Uniform::EdgeDetection_pass2()
{
    Shader_EdgeDetection.setUniform("Pass", 2);

    //Binding Frame Buffer to Texture 11
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, EDrenderTex);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices(Shader_EdgeDetection);

    //Drawing Quad
    glBindVertexArray(EDquad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void SceneBasic_Uniform::EdgeDetection_setUpFBO()
{
    //Generate and bind the framebuffer
    glGenFramebuffers(1, &EDfboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, EDfboHandle);

    glGenTextures(1, &EDrenderTex);
    glBindTexture(GL_TEXTURE_2D, EDrenderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EDrenderTex, 0);

    //Create Depth Buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//  --- Smoke Particle Effect ---
void SceneBasic_Uniform::ParticleEffects_init()
{
    //Setting Variables. Could be completed in constructor, Better for OCD to do it here.
    PEdrawBuf = 1;
    PEnParticles = 4000;
    PEemitterPos = vec3(1, 0, 0);
    PEparticleLifetime = 6.0f;
    PEemitterDir = vec3(0, 2, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Setting gl Info as could have been reset in other shaders.
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);

    //Setting proj/view as differnt to other shaders.
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);
    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Enable blend so can use PNG's etc.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);

    //Loading both potential Textures.
    glActiveTexture(GL_TEXTURE12);
    Texture::loadTexture("media/texture/smoke.png");
    glActiveTexture(GL_TEXTURE14);
    Texture::loadTexture("media/texture/fire.png");

    //Setting noise particles based on amount
    glActiveTexture(GL_TEXTURE13);
    ParticleUtils::createRandomTex1D(PEnParticles * 3);

    ParticleEffects_initBuffers();

    //Setting Uniforms can be changed through GUIs
    Shader_SmokEffect.use();
    Shader_SmokEffect.setUniform("RandomTex", 13);
    
    Shader_SmokEffect.setUniform("ParticleLifetime", PEparticleLifetime);
    Shader_SmokEffect.setUniform("Accel", vec3(0.0f, 0.5f, 0.0));
    Shader_SmokEffect.setUniform("ParticleSize", 0.1f);
    Shader_SmokEffect.setUniform("Emitter", PEemitterPos);
    Shader_SmokEffect.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(PEemitterDir));
}
void SceneBasic_Uniform::ParticleEffects_initBuffers()
{
    //Creating Feedback  Buffers
    glGenBuffers(2, PEposBuf);
    glGenBuffers(2, PEvelBuf);
    glGenBuffers(2, PEage);

    //
    //  --- This has all been commented on in Mix2, No need to duplicate comments. ---
    //
    int size = PEnParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, PEposBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, PEposBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, PEvelBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, PEvelBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, PEage[0]);
    glBufferData(GL_ARRAY_BUFFER, PEnParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, PEage[1]);
    glBufferData(GL_ARRAY_BUFFER, PEnParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat> tempData(PEnParticles);
    float rate = PEparticleLifetime / PEnParticles;

    for (int i = 0; i < PEnParticles; i++)
    {
        tempData[i] = rate * (i - PEnParticles);
    }

    glBindBuffer(GL_ARRAY_BUFFER, PEage[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, PEnParticles * sizeof(float), tempData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(2, PEparticleArray);

    glBindVertexArray(PEparticleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, PEposBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, PEvelBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, PEage[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);


    glBindVertexArray(PEparticleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, PEposBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, PEvelBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, PEage[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glGenTransformFeedbacks(2, PEfeedback);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, PEfeedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, PEposBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, PEvelBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, PEage[0]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, PEfeedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, PEposBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, PEvelBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, PEage[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}
void SceneBasic_Uniform::ParticleEffects_render()
{
    Shader_SmokEffect.use();

    //Updating time so position of particles can be calculated.
    Shader_SmokEffect.setUniform("Time", time);
    Shader_SmokEffect.setUniform("DeltaT", deltaT);

    //Setting Texture based on fire or smoke wanted.
    if (FireOrSmoke)
    {
        Shader_SmokEffect.setUniform("ParticleTex", 14);
    }
    else
    {
        Shader_SmokEffect.setUniform("ParticleTex", 12);
    }

    //Setting Uniforms that work out "Length" of smoke particles base
    Shader_SmokEffect.setUniform("left", smokeLeft);
    Shader_SmokEffect.setUniform("right", smokeRight);
    
    Shader_SmokEffect.setUniform("Pass", 1);

    //
    //  --- This has all been commented on in Mix2, No need to duplicate comments. ---
    //

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, PEfeedback[PEdrawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(PEparticleArray[1 - PEdrawBuf]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);

    glDrawArrays(GL_POINTS, 0, PEnParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    
    Shader_SmokEffect.setUniform("Pass", 2);
    view = glm::lookAt(CameraPos, vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    model = mat4(1.0f);
    model = glm::translate(model, vec3(SmokeFireTrans[0], SmokeFireTrans[1], SmokeFireTrans[2]));
    setMatrices(Shader_SmokEffect);

    glDepthMask(GL_FALSE);
    glBindVertexArray(PEparticleArray[PEdrawBuf]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, PEnParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    PEdrawBuf = 1 - PEdrawBuf;
}


//  --- Custom Object Effect ---
void SceneBasic_Uniform::CustomParticle_init()
{
    //Setting Inital Values. Could be put into Constructor. OCD prefers them like this! :)
    CPnParticles = 50;
    CPparticleLifetime = 10.5f;
    CPdrawBuf = 1;
    CPemitterPos = vec3(0.0f);
    CPemitterDir = vec3(0, 1, 0);
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);

    //Setting GL variables as other shaders can change this.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    //Creating random texture and binding to 15
    ParticleUtils::createRandomTex1D(CPnParticles * 4);
    glActiveTexture(GL_TEXTURE15);

    model = mat4(1.0f);
    CustomParticle_initBuffers();

    //Setting Uniforms, Can be changed through imGUI
    Shader_CustomParticle.use();
    Shader_CustomParticle.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    Shader_CustomParticle.setUniform("ParticleLifetime", CPparticleLifetime);
    Shader_CustomParticle.setUniform("Accel", vec3(0.0f, 5.0f, 0.0));
    Shader_CustomParticle.setUniform("Emitter", CPemitterPos);
    Shader_CustomParticle.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(CPemitterDir));
}
void SceneBasic_Uniform::CustomParticle_render()
{
    //
    //  --- This has all been commented on in Mix2, No need to duplicate comments. ---
    //
    Shader_CustomParticle.use();

    Shader_CustomParticle.setUniform("Time", time);
    Shader_CustomParticle.setUniform("DeltaT", deltaT);
    Shader_CustomParticle.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, CPfeedback[CPdrawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(CPparticleArray[1 - CPdrawBuf]);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glVertexAttribDivisor(3, 0);
    glVertexAttribDivisor(4, 0);
    glVertexAttribDivisor(5, 0);
    glVertexAttribDivisor(6, 0);

    glDrawArrays(GL_POINTS, 0, CPnParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    Shader_CustomParticle.setUniform("Pass", 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view = glm::lookAt(vec3(3.0 * cos(90), 1.5f, 3.0f * sin(90)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    Shader_CustomParticle.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Shader_CustomParticle.setUniform("Material.Kd", 0.2f, 0.5, 0.9f);
    Shader_CustomParticle.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    Shader_CustomParticle.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    Shader_CustomParticle.setUniform("Material.E", 0.0f, 0.0f, 0.0f);
    Shader_CustomParticle.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(0.1f));
    setMatrices(Shader_CustomParticle);

    glBindVertexArray(CPparticleArray[CPdrawBuf]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glDrawElementsInstanced(GL_TRIANGLES, Current.get()->getNumVerts(), GL_UNSIGNED_INT, 0, CPnParticles);

    Shader_CustomParticle.setUniform("Material.Kd", 0.0f, 0.0, 0.0f);
    Shader_CustomParticle.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    Shader_CustomParticle.setUniform("Material.Ka", 0.0f, 0.0f, 0.0f);
    Shader_CustomParticle.setUniform("Material.E", 0.2f, 0.2f, 0.2f);
    Shader_CustomParticle.setUniform("Material.Shininess", 1.0f);

    CPdrawBuf = 1 - CPdrawBuf;
}
void SceneBasic_Uniform::CustomParticle_initBuffers()
{
    //
    //  --- This has all been commented on in Mix2, No need to duplicate comments. ---
    //

    glGenBuffers(2, CPposBuf);
    glGenBuffers(2, CPvelBuf);
    glGenBuffers(2, CPage);
    glGenBuffers(2, CProtation);

    int size = CPnParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, CPposBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, CPposBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, CPvelBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, CPvelBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, CPage[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, CPage[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

    glBindBuffer(GL_ARRAY_BUFFER, CProtation[0]);
    glBufferData(GL_ARRAY_BUFFER, 2 * size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, CProtation[1]);
    glBufferData(GL_ARRAY_BUFFER, 2 * size, 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat> initialAges(CPnParticles);
    float rate = CPparticleLifetime / CPnParticles;

    for (int i = 0; i < CPnParticles; i++)
    {
        initialAges[i] = rate * (i - CPnParticles);
    }

    glBindBuffer(GL_ARRAY_BUFFER, CPage[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, CPnParticles * sizeof(float), initialAges.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(2, CPparticleArray);

    glBindVertexArray(CPparticleArray[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Current.get()->getElementBuffer());

    glBindBuffer(GL_ARRAY_BUFFER, Current.get()->getPositionBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, Current.get()->getNormalBuffer());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, CPposBuf[0]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, CPvelBuf[0]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, CPage[0]);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, CProtation[0]);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(6);


    glBindVertexArray(CPparticleArray[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Current.get()->getElementBuffer());

    glBindBuffer(GL_ARRAY_BUFFER, Current.get()->getPositionBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, Current.get()->getNormalBuffer());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, CPposBuf[1]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, CPvelBuf[1]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, CPage[1]);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, CProtation[1]);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);

    glGenTransformFeedbacks(2, CPfeedback);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, CPfeedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, CPposBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, CPvelBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, CPage[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, CProtation[0]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, CPfeedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, CPposBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, CPvelBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, CPage[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, CProtation[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

}

//  --- General ---
void SceneBasic_Uniform::RenderSkyBox()
{
    //Reset Model
    model = mat4(1.0f);

    //Set Matrices and renders skybox... Simple.
    setMatrices(Shader_SkyBox);
    sky.render();
}
void SceneBasic_Uniform::GeneralRender(GLSLProgram& prog)
{
    //Setting GL variables which could be changed by other shaders.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);
    view = glm::lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    //Setting models Scale, Position and Rotation, Could be auto rotation.
    model = mat4(1.0f);
    model = glm::translate(model, ObjTransformVec);
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
        model = glm::rotate(model, glm::radians((float)angle_1 * AutoRotateSpeed), vec3(0.0f, 1.0f, 0.0f));

    }
    setMatrices(prog);
    Current->render();

    glFinish();
}
void SceneBasic_Uniform::OffSwitcher(int i)
{
    //Pass a number into this Method.
    //Turns off all shaders except one.
    //Stops Repeating code.

    // --- Pass in Enables --
    //1 - Mix1
    //2 - Mix2
    //3 - WireFrame
    //4 - Night Vision
    //5 - Silhouette
    //6 - Gaussing
    //7 - Edge Detection
    //9 - Custom Particle System

    if (i != 1) { show_Mix1 = false; }
    if (i != 2) { show_Mix2 = false; }
    if (i != 3) { show_Wireframe = false; }
    if (i != 4) { show_NightVision = false; }
    if (i != 5) { show_Silhouette = false; }
    if (i != 6) { show_Gaussin = false; }
    if (i != 7) { show_edgeDetection = false; }
    if (i != 8) { show_customParticle = false; }
}

//  --- ImGUI Based Methods ---
//Do not feel like i need to comment on all of these. Alot of this is duplication for other shader. 
//If you need more info for these. Contact me and ill talk for hours about ImGUI!
void SceneBasic_Uniform::GUIMenuRender()
{
    //  --- Menu At Top Of Application ---
    //Creating New Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        //Each Menu Listed Here.
        //Inside sub menu inside, toggles bool which runs Renders + windows
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Open", "Open .obj File", &show_Open_File);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Object"))
        {
            ImGui::MenuItem("Transfrom", "Move Object", &show_transform);
            ImGui::MenuItem("Scale", "Scale Object", &show_Scale);
            ImGui::MenuItem("Rotation", "Rotate Object", &show_Rotation);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Shaders"))
        {
            if (ImGui::MenuItem("Shadow + Silhouette", "Shadow Volume + Silhouette Mix", &show_Mix1))
            {
                LoadObjectAdj();
                OffSwitcher(1);
            }
            if (ImGui::MenuItem("Shadow + Particles", "Shadow Soft + Particle System", &show_Mix2))
            {
                OffSwitcher(2);
                Mix2_init();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("WireFrame", "View Objects Wireframe", &show_Wireframe)) {
                LoadObject();
                OffSwitcher(3);
            }
            if (ImGui::MenuItem("Night Vision", "View Objects in Night Vision", &show_NightVision)) {
                LoadObject();
                OffSwitcher(4);
                NightVision_init();
            }
            if (ImGui::MenuItem("Gaussin Blur", "Not Active", &show_Gaussin)) {
                LoadObject();
                OffSwitcher(6);
                Gaussin_init();
            }
            if (ImGui::MenuItem("Edge Detection", "View Edges", &show_edgeDetection)) {
                LoadObject();
                OffSwitcher(7);
                EdgeDetection_init();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Silhouette", "View Objects in Silhouette", &show_Silhouette)) {
                LoadObjectAdj();
                OffSwitcher(5);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Backgrounds"))
        {
            if (ImGui::MenuItem("Clouds", "Cloud Background", &show_Clouds))
            {
                show_Wood = false;
                show_skyBox = false;
            }
            if (ImGui::MenuItem("Wood", "Wood Background", &show_Wood))
            {
                show_Clouds = false;
                show_skyBox = false;
            }
            if (ImGui::MenuItem("SkyBox", "SkyBox Background", &show_skyBox))
            {
                show_Clouds = false;
                show_Wood = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Lighting"))
        {
            if (ImGui::MenuItem("Light Position", "Change Position of Light", &show_LightPos)) {};
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Extras"))
        {
            if (ImGui::MenuItem("Smoke Effects", "Add into Scene", &show_particleEffects)) { ParticleEffects_init(); };
            if (ImGui::MenuItem("Custom OBJ Particles", "Change to Scene", &show_customParticle))
            {
                LoadObject();
                OffSwitcher(8);
                CustomParticle_init();

            };
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
void SceneBasic_Uniform::GUIWindowRender()
{
    //  --- GUI Window Rendering.
    //Opening Object/Texture Window
    if (show_Open_File)             { ShowOpenFile(); }

    //  --- Obejcts Transform Windows ---
    if (show_transform)             { ShowTransformWindow(); }
    if (show_Scale)                 { ShowScaleWindow(); }
    if (show_Rotation)              { ShowRotationWindow(); }

    //   --- Shader Editing Windows ---
    if (show_Wireframe)             { ShowWireframeWindow(); }
    if (show_Silhouette)            { ShowSilhouetteWindow(); }
    if (show_Mix1)                  { ShowMix1Window(); }
    if (show_Mix2)                  { ShowMix2Window(); }
    if (show_Gaussin)               { ShowGaussinWindow(); }
    if (show_particleEffects)       { ShowSmokeFireWindow(); }
    if (show_customParticle)        { showCustomPrticleWindow(); }

    //  --- Background Windows --- 
    if (show_Clouds)                { ShowCloudsWindow(); }
    if (show_Wood)                  { ShowWoodWindow(); }
}
void SceneBasic_Uniform::ShowOpenFile()
{
    //  --- Opening Texture/Object Window ---
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoCollapse;

    {
        //GUI for entering Strings.
        ImGui::Begin("Open File", &show_Open_File, OpenFileTags);
        ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.9f, 1.0f), "Put all infomation into media folder.");
        ImGui::InputText(".obj File - Object", fileInput, 64, ImGuiInputTextFlags_CharsNoBlank);
        ImGui::InputText(".png File - Texture", fileInputTex, 64, ImGuiInputTextFlags_CharsNoBlank);

        //If Opened Set Locations as new Data
        if (ImGui::Button("Open"))
        {
            string NewTexture(fileInputTex);
            string NewObject(fileInput);
            if (NewTexture != "")
            {
                TextureLocation = NewTexture;
            }

            ObjLocation = NewObject;
            NewFileLoad = true;

            //Load New Objects As Adj or normal.
            if (AdjLoaded == true)
            {
                LoadObjectAdj();
            }
            else
            {
                LoadObject();
            }
        }

        //Opening Files Examples.
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), "Try 'Cow' for Both, As another example!");
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), "Or 'SportsCar' as Obj and 'SportsCar1', 2 or 3 as PNG!");
        ImGui::End();
    }
}

void SceneBasic_Uniform::ShowTransformWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Transform Object", &show_transform, OpenFileTags);
        ImGui::Text("Position X Y Z");
        ImGui::SliderFloat("X", &ObjTransform[0], -10.0f, 10.0f);
        ImGui::SliderFloat("Y", &ObjTransform[1], -10.0f, 10.0f);
        ImGui::SliderFloat("Z", &ObjTransform[2], -10.0f, 10.0f);

        if (ImGui::Button("Reset X")) { ObjTransform[0] = 0.0f; } ImGui::SameLine();
        if (ImGui::Button("Reset Y")) { ObjTransform[1] = 0.0f; } ImGui::SameLine();
        if (ImGui::Button("Reset Z")) { ObjTransform[2] = 0.0f; }

        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowScaleWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Scale Object", &show_Scale, OpenFileTags);
        ImGui::Text("Scale Object");
        ImGui::SliderFloat("Size", &ObjScale, 0.2f, 10.0f);
        if (ImGui::Button("Reset Size")) { ObjScale = 0.8f; }

        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowRotationWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Rotate Object", &show_Rotation, OpenFileTags);
        ImGui::Text("Rotate Object");
        ImGui::SliderAngle("X Angle", &ObjRotationX);
        ImGui::SliderAngle("Y Angle", &ObjRotationY);
        ImGui::SliderAngle("Z Angle", &ObjRotationZ);


        if (ImGui::Button("Reset X")) { ObjRotationX = 0.0f; } ImGui::SameLine();
        if (ImGui::Button("Reset Y")) { ObjRotationY = 0.0f; } ImGui::SameLine();
        if (ImGui::Button("Reset Z")) { ObjRotationZ = 0.0f; }

        ImGui::Checkbox("Auto Rotate!", &AutoRotate);

        ImGui::End();
    }
}

void SceneBasic_Uniform::ShowMix1Window()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Shadow Volume + Silhouette", &show_Mix1, OpenFileTags);
        ImGui::Text("Settings");
        ImGui::SliderFloat("Edge Width", &Mix1EW, 0.0, 0.5);
        ImGui::SliderFloat("PCT Extend", &Mix1PE, 0.0, 0.5);
        ImGui::SliderInt("Toons Levels", &Mix1Levels, 1, 20);
        if (ImGui::Button("Reset Edge")) { Mix1EW = 0.004; } ImGui::SameLine();
        if (ImGui::Button("Reset PCT")) { Mix1PE = 0.00; } ImGui::SameLine();
        if (ImGui::Button("Reset Levels")) { Mix1Levels = 10; }

        ImGui::ColorEdit3("Light Colour", (float*)&Mix1Color);
        if (ImGui::Button("Reset Colour")) { Mix1Color = vec4(1.0f); }

        ImGui::Checkbox("Enable Toon Shading", &toonShadingActive);
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), "Change Light Position From Menu");
        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowMix2Window()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Shadow Map Soft + Patricle System", &show_Mix2, OpenFileTags);
        ImGui::Text("Silhouette Settings");
        ImGui::SliderInt("Lifetime", &Mix2NewPLife, 1, 10);
        ImGui::SliderInt("Amount", &Mix2NewPAmount, 10, 200);
        ImGui::SliderFloat("Speed", &Mix2AccelAmount, 1.5, -1.5);
        if (ImGui::Button("Reset Life")) { Mix2NewPLife = 8; } ImGui::SameLine();
        if (ImGui::Button("Reset Amount")) { Mix2NewPAmount = 100; } ImGui::SameLine();
        if (ImGui::Button("Reset Speed")) { Mix2AccelAmount = -0.4; }

        ImGui::ColorEdit3("Light-Colour", (float*)&Mix2LightCol);
        if (ImGui::Button("Reset Colour")) { Mix2LightCol = vec4(1.0f); }

        if (ImGui::Button("Apply"))
        {
            Mix2_init();
            Mix2_nParticles = Mix2NewPAmount;
            Mix2_particleLifetime = float(Mix2NewPLife);
        } ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), " <-- Apply To Update!");

        ImGui::End();
    }
}

void SceneBasic_Uniform::ShowLightPosWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Light Settings", &show_LightPos, OpenFileTags);
        ImGui::SliderFloat("Angle", &LightAngle, 0.00, 6.25f);
        if (ImGui::Button("Rest Light"))
        {
            LightAngle = 1.0f;
        }

        ImGui::Checkbox("Auto Rotate", &Light_AutoRotate);

        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowWireframeWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Wireframe Options", &show_Rotation, OpenFileTags);
        ImGui::Text("Line Settings");
        ImGui::SliderFloat("Line Width", &ObjWFWidth, 0.20f, 2.0f);
        ImGui::ColorEdit3("Line Colour", (float*)&ObjWFColor);
        ImGui::ColorEdit3("Back Colour", (float*)&ObjWFBack);

        if (ImGui::Button("Reset Width"))
        {
            ObjWFWidth = 0.75f;
        } ImGui::SameLine();
        if (ImGui::Button("Reset Line"))
        {
            ObjWFColor = vec4(0.05, 0.0f, 0.05f, 1.0f);
        }ImGui::SameLine();
        if (ImGui::Button("Reset Back"))
        {
            ObjWFBack = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowCloudsWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Cloud Options", &show_Clouds, OpenFileTags);
        ImGui::Text("Cloud Settings");

        ImGui::ColorEdit3("Cloud Colour", (float*)&Clouds_Cloud);
        ImGui::ColorEdit3("Sky Colour", (float*)&Clouds_Sky);
        if (ImGui::Button("Reset Cloud")) { Clouds_Cloud = vec4(1.0, 1.0, 1.0, 1.0); } ImGui::SameLine();
        if (ImGui::Button("Reset Sky")) { Clouds_Sky = vec4(0.1, 0.3, 0.9, 1.0); }

        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowWoodWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Wood Options", &show_Clouds, OpenFileTags);
        ImGui::Text("Wood Settings");
        ImGui::ColorEdit3("First Colour", (float*)&DarkWoodColor);
        ImGui::ColorEdit3("Second Colour", (float*)&LightWoodColor);
        if (ImGui::Button("Reset First")) { DarkWoodColor = vec4(0.8, 0.5, 0.1, 1.0); } ImGui::SameLine();
        if (ImGui::Button("Reset Second")) { LightWoodColor = vec4(1.0, 0.75, 0.25, 1.0); }

        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowGaussinWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Gaussin Options", &show_Gaussin, OpenFileTags);
        ImGui::SliderFloat("Blur", &GaussingBlurAmount, 0.1f, 20.0f);
        if (ImGui::Button("Reset Blur")) { GaussingBlurAmount = 10; } ImGui::SameLine();
        if (ImGui::Button("Apply Blur")) { Gaussin_init(); }ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), " <-- Apply");
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), " Now With Textures!");
        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowSmokeFireWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;

    {
        ImGui::Begin("Smoke/Fire Settings");
        ImGui::Checkbox("Enable Transform", &SmokeFireTransform);

        if (SmokeFireTransform)
        {
            ImGui::SliderFloat("-X-", &SmokeFireTrans[0], -5.0f, 5.0f);
            ImGui::SliderFloat("-Y-", &SmokeFireTrans[1], -5.0f, 5.0f);
            ImGui::SliderFloat("-Z-", &SmokeFireTrans[2], -5.0f, 5.0f);
            if (ImGui::Button("Reset X")) { SmokeFireTrans[0] = -1.0f; }
            if (ImGui::Button("Reset Y")) { SmokeFireTrans[1] = -1.5f; }
            if (ImGui::Button("Reset Z")) { SmokeFireTrans[2] = -3.0f; }
        }

        ImGui::SliderFloat("Width Left", &smokeLeft, -1.0f, -8.0f);
        ImGui::SliderFloat("Width Right", &smokeRight, 1.0f, 8.0f);
        if (ImGui::Button("Reset Left")) { smokeLeft = -2.0f; } ImGui::SameLine();
        if (ImGui::Button("Reset Right")) { smokeRight = 2.0f; }
        if (ImGui::Button("Fire")) { FireOrSmoke = true; } ImGui::SameLine();
        if (ImGui::Button("Smoke")) { FireOrSmoke = false; }
        ImGui::End();
    }
}
void SceneBasic_Uniform::showCustomPrticleWindow()
{
    {
        ImGui::Begin("Custom Object Particles.");
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), "To Correctly Load, Go to WireFrame. Then Go To This Menu!\nShould Create a Line of Objects, Not all ontop of each other!");
        ImGui::End();
    }
}
void SceneBasic_Uniform::ShowSilhouetteWindow()
{
    ImGuiWindowFlags OpenFileTags = 0;
    OpenFileTags |= ImGuiWindowFlags_NoScrollbar;

    {
        ImGui::Begin("Silhouette", &show_Silhouette, OpenFileTags);
        ImGui::Text("Settings");
        ImGui::SliderFloat("Edge-Width", &Sil_EdgeWidth, 0.0, 0.5);
        ImGui::SliderFloat("PCT-Extend", &Sil_PctExtend, 0.0, 0.5);
        ImGui::SliderInt("Toons-Levels", &Sil_Levels, 1, 20);
        if (ImGui::Button("Reset-Edge")) { Sil_EdgeWidth = 0.004; } ImGui::SameLine();
        if (ImGui::Button("Reset-PCT")) { Sil_PctExtend = 0.00; } ImGui::SameLine();
        if (ImGui::Button("Reset-Levels")) { Sil_Levels = 10; }

        ImGui::ColorEdit3("Light-Colour", (float*)&Sil_LightColor);
        if (ImGui::Button("Reset-Colour")) { Sil_LightColor = vec3(0.0f, 0.83f, 0.63f); }

        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), "Change Object Rotation From Menu");
        ImGui::End();
    }
}