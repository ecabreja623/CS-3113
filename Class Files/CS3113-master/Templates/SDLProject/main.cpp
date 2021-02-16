#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"              //allows you to load an image into a texture

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

float player_x = 0;
float player_rotate = 0;

GLuint playerTextureID;

GLuint LoadTexture(const char* filePath) {      // loads texture
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha); // takes image from filepath
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);   // creates texture ID for this image
    glBindTexture(GL_TEXTURE_2D, textureID);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); // loads texture onto video card
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");   // open gl shader language; loads shader that handles textured polygons

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(0.0f, 0.5f, 1.0f, 1.0f);   // sets color of untextured polygon; r,g,b,a; not needed for textured polygons

    glUseProgram(program.programID);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);   // sets color of background; red, green, blue, alpha(opacity)
    glEnable(GL_BLEND); // enables blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //handles transparency

    playerTextureID = LoadTexture("r34gtr.png");    // loads the image onto video card
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() { 

    float ticks = (float)SDL_GetTicks() / 1000.0f;  // seconds since initialization
    float deltaTime = ticks - lastTicks;    // subtracts time since init. from last frame to get deltatime
    lastTicks = ticks;

    //player_x += 1.0f;   // adds 1 to x position every time it updates
    player_x += 1.0f * deltaTime;   // moves 1 unit per second
    player_rotate += -90.0f * deltaTime;    //positive = counter clockwise, negative = clockwise rotation

    modelMatrix = glm::mat4(1.0f);      //matrix is identity matrix, centers triangle every frame if not commented out
    modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.0f, 0.0f));     // move triangle, vec3 is x y z, z used for rotation and 3d
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));    // rotates by certain degree, positive z value = counter clockwise, negative = clockwise rotation
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));   // scale triangle by scalar amount
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetModelMatrix(modelMatrix);
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}