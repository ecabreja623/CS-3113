#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Effects.h"
#include "Entity.h"
#include "Scene.h"
#include "MainMenu.h"
#include "VehicleSelection.h"
#include "Level1.h"
#include "Level2.h"
#include "EndGameWin.h"
#include "EndGameLoss.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 uiViewMatrix, uiProjectionMatrix;

GLuint fontTextureID;

Effects* effects;

Mix_Music* music;   // looping background music
Mix_Chunk* car_crash_sound;     // sound effect when car crashes

Scene* currentScene;
Scene* sceneList[6];

int previousScene;  // index of previous scene to be able to restart from same scene
int carSelected = -1;   // index of car chosen in vehicle selection scene

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize(carSelected);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("No Hesi!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);  // change aspect ratio
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 1280, 720);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS("tension-sleepyhallow.mp3");

    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

    car_crash_sound = Mix_LoadWAV("car_crash.wav");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 200.0f);     // used for 3d pov 1.777 = 1280/720, 0.1f = near plane distance, 100.0f = draw distance

    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);

    fontTextureID = Util::LoadTexture("font2.png");

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);    // allows comparison to z-buffer (checks depth of two pixels)
    glDepthMask(GL_TRUE);   // enables writing to depth buffer
    glDepthFunc(GL_LEQUAL); // depth function is <=, draws the pixel closer to screen

    sceneList[0] = new MainMenu();
    sceneList[1] = new VehicleSelection();
    sceneList[2] = new Level1();
    sceneList[3] = new Level2();
    sceneList[4] = new EndGameWin();
    sceneList[5] = new EndGameLoss();

    SwitchToScene(sceneList[0]);

    effects = new Effects(uiProjectionMatrix, uiViewMatrix);
    effects->Start(FADEIN, 0.5f);
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
            case SDLK_RETURN:
                if (currentScene == sceneList[0] || currentScene == sceneList[4] || currentScene == sceneList[5]) {
                    currentScene->state.nextScene = 1;  // press ENTER to start or restart game
                    break;
                }
                else if (currentScene == sceneList[1]) {    // press ENTER to slect car
                    carSelected = currentScene->state.carSelected;  // store car selected value and go to next scene
                    currentScene->state.nextScene = 2;
                }
            case SDLK_SPACE:
                if (currentScene == sceneList[5]) { // press SPACE to restart from this level
                    currentScene->state.nextScene = previousScene;
                    break;
                }
            case SDLK_RIGHT:
                if (currentScene == sceneList[1]) { // scroll through cars in vehicle selection
                    currentScene->state.carSelected = (currentScene->state.carSelected + 1) % 3;
                }
                break;
            case SDLK_LEFT:
                if (currentScene == sceneList[1]) {
                    currentScene->state.carSelected = (currentScene->state.carSelected - 1) % 3;
                }
                break;
    
            }
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (currentScene != sceneList[0] && currentScene != sceneList[1] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
        
        if (keys[SDL_SCANCODE_A]) {
            currentScene->state.player->rotation.z += 3;   // turn left when holding A
        }
        else if (keys[SDL_SCANCODE_D]) {
            currentScene->state.player->rotation.z -= 3;   // turn right when holding D
        }

        currentScene->state.player->velocity.x = 0;
        currentScene->state.player->velocity.z = 0;

        if (keys[SDL_SCANCODE_W]) { // W to drive in the direction you are facing
            currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.z)) * -2.0f;
            currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.z)) * -2.0f;
        }
        else if (keys[SDL_SCANCODE_S]) {    // S to reverse
            currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.z)) * 2.0f;
            currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.z)) * 2.0f;
        }
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

        currentScene->Update(FIXED_TIMESTEP);

        if (currentScene != sceneList[0] && currentScene != sceneList[1] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
            if (currentScene->state.player->crashed) {  // play sound effect if crashed
                Mix_PlayChannel(-1, car_crash_sound, 0);
            }
        }

        effects->Update(FIXED_TIMESTEP);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);

    if (currentScene != sceneList[0] && currentScene != sceneList[1] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
        viewMatrix = glm::translate(viewMatrix, -glm::vec3(currentScene->state.player->position.x, currentScene->state.player->position.y + 2, currentScene->state.player->position.z + 10));
    }   // third person pov 
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    if (currentScene != sceneList[0] || currentScene != sceneList[5]) { // render 3D scenes only
        currentScene->Render(&program);
    }

    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);

    if (currentScene == sceneList[0] || currentScene == sceneList[5]) { // render non 3D scenes only or 2D UI
        currentScene->Render(&program);
    }
    else if (currentScene == sceneList[2] || currentScene == sceneList[3]){
        int distance;
        if (currentScene == sceneList[2]) {
            distance = currentScene->state.player->position.z + 390;    // remaining distance until level 1 is complete
        }
        else {
            distance = currentScene->state.player->position.z + 540;    // remaining distance until level 2 is complete
        }
        
        Util::DrawIcon(&program, Util::LoadTexture("game_logo.png"), glm::vec3(5.5, 3.2, 0));   // logo on top right corner

        Util::DrawText(&program, Util::LoadTexture("font2.png"), "Distance Left: " + std::to_string(distance), 0.5, -0.3f, glm::vec3(-6, 3.2, 0));   // remaining distance top left corner

        Util::DrawText(&program, Util::LoadTexture("font2.png"), std::to_string(int(-currentScene->state.player->velocity_mph)) + " mph", 0.5, -0.3f, glm::vec3(5, -3, 0)); // mph in bottom right corner
    }
    else if (currentScene == sceneList[1]) {  // scene [1] - Vehicle Selection
        Util::DrawText(&program, Util::LoadTexture("font2.png"), "SELECT A VEHICLE", 0.5f, 0.0f, glm::vec3(-3.5, 3, 0));
        Util::DrawText(&program, Util::LoadTexture("font2.png"), "Use arrow keys and ENTER to select", 0.25f, 0.0f, glm::vec3(-4, -3, 0));
    }
    else if (currentScene == sceneList[4]) {
        Util::DrawIcon(&program, Util::LoadTexture("game_logo.png"), glm::vec3(0, 3.2, 0));

        Util::DrawText(&program, Util::LoadTexture("font2.png"), "YOU WIN!", 1.0f, 0.0f, glm::vec3(-3.25, 2, 0));
        Util::DrawText(&program, Util::LoadTexture("font2.png"), "YOU WERE CUTTING LIKE A KNIFE!", 0.35f, 0.0f, glm::vec3(-5, 0.75, 0));

        Util::DrawText(&program, Util::LoadTexture("font2.png"), "PRESS ENTER TO PLAY AGAIN", 0.25f, 0.0f, glm::vec3(-2.85, 0, 0));
    }

    effects->Render();

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

        if (currentScene->state.nextScene >= 0) { // if switching level

            if (currentScene == sceneList[2]) { // keep track of what was the previous scene
                previousScene = 2;
            }
            else if (currentScene == sceneList[3]) {
                previousScene = 3;
            }
            
            effects->Start(FADEOUT, 0.5f);  // fade out of old level
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            effects->Start(FADEIN, 0.5f);   // fade in to new level
        }

        Render();
    }

    Shutdown();
    return 0;
}
