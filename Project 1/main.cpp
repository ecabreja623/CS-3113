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
#include <vector>

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, gtrMatrix, bmwMatrix, porscheMatrix, projectionMatrix;

float gtr_x = 0;
float gtr_y = 0;
float gtr_rotate = 0;
float gtr_size = 0;

GLuint gtrTextureID;

float bmw_x = 0;
float bmw_y = 0;
float bmw_rotate = 0;
float bmw_size = 0;

GLuint bmwTextureID;

float porsche_x = 0;
float porsche_y = 0;
float porsche_rotate = 0;
float porsche_size = 0;

GLuint porscheTextureID;

GLuint fontTextureID;

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

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
offset + (-0.5f * size), 0.5f * size,
offset + (-0.5f * size), -0.5f * size,
offset + (0.5f * size), 0.5f * size,
offset + (0.5f * size), -0.5f * size,
offset + (0.5f * size), 0.5f * size,
offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
        u, v,
        u, v + height,
        u + width, v,
        u + width, v + height,
        u + width, v,
        u, v + height,
            });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Vroom!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");   // open gl shader language; loads shader that handles textured polygons

    viewMatrix = glm::mat4(1.0f);

    gtrMatrix = glm::mat4(1.0f);
    bmwMatrix = glm::mat4(1.0f);
    porscheMatrix = glm::mat4(1.0f);

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(0.0f, 0.5f, 1.0f, 1.0f);   // sets color of untextured polygon; r,g,b,a; not needed for textured polygons

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);   // sets color of background; red, green, blue, alpha(opacity)
    glEnable(GL_BLEND); // enables blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //handles transparency

    gtrTextureID = LoadTexture("r34gtr.png");    // loads the image onto video card
    bmwTextureID = LoadTexture("e36bmw.png");
    porscheTextureID = LoadTexture("porsche911.png");
    fontTextureID = LoadTexture("pixel_font.png");
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
    gtr_x += 1.0f * deltaTime;   // moves 1 unit per second
    gtr_rotate += -90.0f * deltaTime;    // positive = counter clockwise, negative = clockwise rotation
    gtr_size += 1.0f * deltaTime;    // scale up

    gtrMatrix = glm::mat4(1.0f);      // matrix is identity matrix
    //gtrMatrix = glm::translate(gtrMatrix, glm::vec3(gtr_x, 0.0f, 0.0f));     // move triangle, vec3 is x y z, z used for rotation and 3d
    gtrMatrix = glm::rotate(gtrMatrix, glm::radians(gtr_rotate), glm::vec3(0.0f, 0.0f, 1.0f));    // rotates by certain degree, positive z value = counter clockwise, negative = clockwise rotation
    gtrMatrix = glm::scale(gtrMatrix, glm::vec3(gtr_size, gtr_size, 1.0f));   // scale triangle by scalar amount

    bmw_x += -0.5f * deltaTime;
    bmw_y += 0.5f * deltaTime;
    bmw_size += 1.0f * deltaTime;

    bmwMatrix = glm::mat4(1.0f);
    bmwMatrix = glm::translate(bmwMatrix, glm::vec3(bmw_x, bmw_y, 0.0f));

    while (porsche_x < 100) {
        porsche_x += 1.0f * deltaTime;


    }
    
    porsche_rotate = 90.f;

    porscheMatrix = glm::mat4(1.0f);
    porscheMatrix = glm::translate(porscheMatrix, glm::vec3(porsche_x, porsche_y, 0.0f));
    porscheMatrix = glm::rotate(porscheMatrix, glm::radians(porsche_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    porscheMatrix = glm::translate(porscheMatrix, glm::vec3(porsche_x, porsche_y, 0.0f));
}

void DrawCar(glm::mat4 matrix, GLuint textureID) {
    program.SetModelMatrix(matrix);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    DrawText(&program, fontTextureID, "TESTING", 0.3f, 0.1f, glm::vec3(0));

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    DrawCar(gtrMatrix, gtrTextureID);
    DrawCar(bmwMatrix, bmwTextureID);
    DrawCar(porscheMatrix, porscheTextureID);

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