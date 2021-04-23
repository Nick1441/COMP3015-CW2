
#include <glad/glad.h>
#include "scene.h"
#include "glutils.h"
#include <GLFW/glfw3.h>
#include "../imGUI/imgui.h"
#include "../imGUI/imgui_impl_glfw_gl3.h"
//#include "../gl3w/gl3w.h"
#include <stdio.h>
//#include "../gl3w/glcorearb.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <fstream>
#include <iostream>

class SceneRunner {
private:
    GLFWwindow * window;
    int fbw, fbh;
	bool debug;           // Set true to enable debug messages

public:
    SceneRunner(const std::string & windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0) : debug(true) {
        
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );


#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        if(debug) 
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        // Open the window
        window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        
  
        // Get framebuffer size
        glfwGetFramebufferSize(window, &fbw, &fbh);

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); }

        GLUtils::dumpGLInfo();

        ImGui_ImplGlfwGL3_Init(window, true);
        

        // Initialization
        glClearColor(0.5f,0.5f,0.5f,1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif
    }

    int run(Scene & scene) {
        scene.setDimensions(fbw, fbh);
        scene.initScene();
        scene.resize(fbw, fbh);

        // Enter the main loop
        mainLoop(window, scene);

#ifndef __APPLE__
		if( debug )
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

		// Close window and terminate GLFW
		glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs(int argc, char ** argv, std::map<std::string, std::string> & sceneData) {
        if( argc < 2 ) {
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        std::string recipeName = argv[1];
        auto it = sceneData.find(recipeName);
        if( it == sceneData.end() ) {
            printf("Unknown recipe: %s\n\n", recipeName.c_str());
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        return recipeName;
    }

private:
    static void printHelpInfo(const char * exeFile,  std::map<std::string, std::string> & sceneData) {
        printf("Usage: %s recipe-name\n\n", exeFile);
        printf("Recipe names: \n");
        for( auto it : sceneData ) {
            printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
        }
    }

    void mainLoop(GLFWwindow * window, Scene & scene) {
        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            GLUtils::checkForOpenGLError(__FILE__,__LINE__);
			
            scene.update(float(glfwGetTime()));
            scene.render();
            
            glfwPollEvents();
           
			int state = glfwGetKey(window, GLFW_KEY_SPACE);

            int Key1 = glfwGetKey(window, GLFW_KEY_1);
            int Key2 = glfwGetKey(window, GLFW_KEY_2);
            int Key3 = glfwGetKey(window, GLFW_KEY_3);
            int Key4 = glfwGetKey(window, GLFW_KEY_4);
            int Key5 = glfwGetKey(window, GLFW_KEY_5);
            int Key6 = glfwGetKey(window, GLFW_KEY_ENTER);
            int Key7 = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
            int Key8 = glfwGetKey(window, GLFW_KEY_ESCAPE);

            //USER INPUT KEYS
            if (state == GLFW_PRESS)
            {
                scene.animate(!scene.animating());
            }
            else if (Key1 == GLFW_PRESS)
            {
                scene.InputPressed(1);
            }
            else if (Key2 == GLFW_PRESS)
            {
                scene.InputPressed(2);
            }
            else if (Key3 == GLFW_PRESS)
            {
                scene.InputPressed(3);
            }
            else if (Key4 == GLFW_PRESS)
            {
                scene.InputPressed(4);
            }
            else if (Key5 == GLFW_PRESS)
            {
                scene.InputPressed(5);
            }
            else if (Key6 == GLFW_PRESS)
            {
                scene.InputPressed(10);
            }
            else if (Key7 == GLFW_PRESS)
            {
                scene.InputPressed(11);
            }
            else if (Key8 == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
            }

            glfwSwapBuffers(window);
        }
        ImGui_ImplGlfwGL3_Shutdown();
    }
};