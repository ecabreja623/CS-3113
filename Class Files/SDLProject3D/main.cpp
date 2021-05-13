#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"

/*      not needed anymore, mesh.cpp handles
float cubeVertices[] = {    // xyz
    -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
    -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5,

    0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5,
    0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5,

    -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5,
    -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5,

    0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5,
    0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5,

    -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5,
    -0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5,

    0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
    0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5
};

float cubeTexCoords[] = {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f
};
*/

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define OBJECT_COUNT 4
#define ENEMY_COUNT 10

struct GameState {
    Entity* player;
    Entity* objects;
    Entity* enemies;
};

GameState state;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("3D!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);  // change aspect ratio
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);    // change aspect ratio

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    //projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);       used for 2d pov
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);     // used for 3d pov 1.777 = 1280/720, 0.1f = near plane distance, 100.0f = draw distance

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);    // allows comparison to z-buffer (checks depth of two pixels)
    glDepthMask(GL_TRUE);   // enables writing to depth buffer
    glDepthFunc(GL_LEQUAL); // depth function is <=, draws the pixel closer to screen


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.5f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;

    state.objects = new Entity[OBJECT_COUNT];

    GLuint floorTextureID = Util::LoadTexture("floor.jpg");

    Mesh* cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("ship.obj", 20);  // loads .obj file, holds values of vertices, tex coords, and vertex normals

    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0, -0.25f, 0);    // ship starts behind our pov
    state.objects[0].rotation = glm::vec3(0, 0, 0);   // turn ship around
    state.objects[0].acceleration = glm::vec3(0, 0, 0);   // ship moves away from us, looks as if flying away
    state.objects[0].scale = glm::vec3(20, 0.5f, 20); // expand x and z and shrink y to make it look flat
    state.objects[0].entityType = FLOOR;

    /*
    state.objects[1].textureID = shipTextureID;
    state.objects[1].mesh = shipMesh;
    state.objects[1].position = glm::vec3(-5, 0, 10);    // ship starts behind our pov
    state.objects[1].rotation = glm::vec3(0, 180, 0);   // turn ship around
    state.objects[1].acceleration = glm::vec3(0, 0, -10);   // ship moves away from us, looks as if flying away
    state.objects[1].entityType = SHIP;
    */
    
    GLuint cubeTextureID = Util::LoadTexture("crate1_diffuse.png");

    Mesh* crateMesh = new Mesh();
    crateMesh->LoadOBJ("cube.obj", 1);  // loads .obj file, holds values of vertices, tex coords, and vertex normals

    state.objects[1].textureID = cubeTextureID;
    state.objects[1].mesh = crateMesh;
    state.objects[1].position = glm::vec3(0, 0.55, -5);
    state.objects[1].entityType = CRATE;

    state.objects[2].textureID = cubeTextureID;
    state.objects[2].mesh = crateMesh;
    state.objects[2].position = glm::vec3(-1, 0.55, -5);
    state.objects[2].entityType = CRATE;

    state.objects[3].textureID = cubeTextureID;
    state.objects[3].mesh = crateMesh;
    state.objects[3].position = glm::vec3(0, 1.55, -5);
    state.objects[3].entityType = CRATE;

    /*
    GLuint marioTextureID = Util::LoadTexture("mario_body.png");

    Mesh* marioMesh = new Mesh();
    marioMesh->LoadOBJ("mario.obj");

    state.objects[1].textureID = marioTextureID;
    state.objects[1].mesh = marioMesh;
    state.objects[1].scale = glm::vec3(0.25f, 0.25f, 0.25f);    // scale mario to 1/4 of regular size
    state.objects[1].position = glm::vec3(-5, 0, -40);
    state.objects[1].entityType = ENEMY;

    GLuint pikachuTextureID = Util::LoadTexture("pikachu.png");

    Mesh* pikachuMesh = new Mesh();
    pikachuMesh->LoadOBJ("pikachu.obj");

    state.objects[2].textureID = pikachuTextureID;
    state.objects[2].mesh = pikachuMesh;
    state.objects[2].position = glm::vec3(2, 0, -3);
    state.objects[2].entityType = ENEMY;
    */

    state.enemies = new Entity[ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(rand() % 20 - 10, 0.55, rand() % 20 - 10);
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
    }
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                // Some sort of action
                break;

            }
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
        state.player->rotation.y += 0.5f;   // rotate/look left when holding A
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->rotation.y -= 0.5f;   // rotate/look right when holding D
    }

    state.player->velocity.x = 0;
    state.player->velocity.z = 0;

    if (keys[SDL_SCANCODE_W]) { // W to walk in the direction you are facing (using unit circle)
        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * -2.0f;
        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * -2.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * 2.0f;
        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * 2.0f;
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT);

        for (int i = 0; i < OBJECT_COUNT; ++i) {
            state.objects[i].Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT);
        }

        for (int i = 0; i < ENEMY_COUNT; ++i) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);   // screen follows where player is looking
    viewMatrix = glm::rotate(viewMatrix,
        glm::radians(state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -state.player->position);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also need to clear depth buffer each frame

    program.SetViewMatrix(viewMatrix);

    //state.player->Render(&program);     //  first person pov, do not need to render player

    for (int i = 0; i < OBJECT_COUNT; ++i) {
        state.objects[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; ++i) {
        state.enemies[i].Render(&program);
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