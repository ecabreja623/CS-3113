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
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define TOTAL_TILE_COUNT 27
#define LAVA_TILE_COUNT 8

struct GameState {
    Entity* player;
    Entity* platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameIsOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
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
    }

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
    displayWindow = SDL_CreateWindow("Lunar Lander!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fontTextureID = LoadTexture("font1.png");

    state.player = new Entity();
    state.player->position = glm::vec3(0.0f,4.0f,0.0f);     // ufo starts at top of screen
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.01f, 0);   // gravity always in effect
    state.player->speed = 1.00f;
    state.player->textureID = LoadTexture("ufoBlue.png");

    state.platforms = new Entity[TOTAL_TILE_COUNT];    // 27 tiles, 2 of which are the safe platforms  

    GLuint lavaTextureID = LoadTexture("platformPack_tile006.png");
    GLuint platformTextureID = LoadTexture("platformPack_tile034.png");
    GLuint safe_platformTextureID = LoadTexture("platformPack_tile028.png");


    float j = -4.5f;    // starting x position of lava floor
    for (int i = 0; i < LAVA_TILE_COUNT; ++i) {
        j++;
        state.platforms[i].textureID = lavaTextureID;
        state.platforms[i].position = glm::vec3(j, -3.25f, 0.0f);   // adds lava texture along bottom of screen
        state.platforms[i].Update(0, NULL, 0);
    }

    j = -4.25;      // starting y position of tiles on sides
    for (int i = LAVA_TILE_COUNT; i < 16; ++i) {    // the next 8 platforms line up the left side of the screen
        j++;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-4.5, j, 0.0f);
        state.platforms[i].Update(0, NULL, 0);
    }

    j = -4.25;
    for (int i = 16; i < 24; ++i) {   // the next 8 platforms line up the right side of the screen
        j++;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(4.5, j, 0.0f);
        state.platforms[i].Update(0, NULL, 0);
    }

    j = -2;
    for (int i = 24; i < TOTAL_TILE_COUNT; ++i) {   // the last 3 platforms are in the middle of the screen
        j++;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(j, 0.75f, 0.0f);
        state.platforms[i].Update(0, NULL, 0);
    }
    state.platforms[3].textureID = safe_platformTextureID;  // these 2 platforms are the safe platforms
    state.platforms[4].textureID = safe_platformTextureID;
}

void ProcessInput() {
    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->acceleration.x = -10.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->acceleration.x = 10.0f;
    }

    if (gameIsOver && keys[SDL_SCANCODE_SPACE]) {   // press SPACE to restart game, after game ends
        state.player->position = glm::vec3(0.0f, 4.0f, 0.0f);   // reset ufo's position
        state.player->movement = glm::vec3(0);
        state.player->velocity = glm::vec3(0);
        state.player->acceleration = glm::vec3(0, -0.01f, 0);
        gameIsOver = false;     // begin updating game again
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
}

#define FIXED_TIMESTEP 0.0166666f   // 60 fps, 1 frame every 0.1666 sec
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;

    if (deltaTime < FIXED_TIMESTEP) {   // if delta time is quicker, accumulate extra time
        accumulator = deltaTime;
        return;
    }

    if (!gameIsOver) {  // do not update when game ends
        while (deltaTime >= FIXED_TIMESTEP) {

            state.player->Update(FIXED_TIMESTEP, state.platforms, TOTAL_TILE_COUNT);

            if (state.player->collided) {    // if ufo touches anything the game ends
                gameIsOver = true;           // result depends on what tile it touches
                break;
            }
            
            deltaTime -= FIXED_TIMESTEP;
        }
    }

    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < TOTAL_TILE_COUNT; ++i) {
        state.platforms[i].Render(&program);
    }

    state.player->Render(&program);

    if (gameIsOver) {   // display result on screen, only occurs if ufo touches a tile

        if (state.player->position.x > -1.25 && state.player->position.x < 1.25 
            && state.player->position.y <= -1.0) 
        {  // if ufo is next to safe platforms when game ends, it can only have landed on safe platforms
            DrawText(&program, fontTextureID, "Mission Successful!", 0.5f, 0.0f, glm::vec3(-4.5f, 1.0f, 0.0f));
            DrawText(&program, fontTextureID, "Press SPACE to play again!", 0.3f, 0.0f, glm::vec3(-3.75f, 0.0f, 0.0f));
        }
        else {  // if not by safe platforms, ufo collided with some other tile
            DrawText(&program, fontTextureID, "Mission Failed!", 0.5f, 0.05f, glm::vec3(-3.5f, 1.0f, 0.0f));
            DrawText(&program, fontTextureID, "Press SPACE to try again!", 0.3f, 0.0f, glm::vec3(-3.5f, 0.0f, 0.0f));
        }
    }
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