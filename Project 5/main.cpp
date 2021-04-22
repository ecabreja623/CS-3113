#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Effects.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "EndGameWin.h"
#include "EndGameLoss.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameIsOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Mix_Music* music;
Mix_Music* mainMenuMusic;
Mix_Chunk* sound_effect;
Mix_Chunk* laser;

Effects* effects;

int lives = 3;

Scene* currentScene;
Scene* sceneList[6];

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("SLATT!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    mainMenuMusic = Mix_LoadMUS("mainMenuMusic.mp3");

    Mix_PlayMusic(mainMenuMusic, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    sound_effect = Mix_LoadWAV("bounce.wav");   
    laser = Mix_LoadWAV("laser.wav");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new MainMenu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new EndGameWin();
    sceneList[5] = new EndGameLoss();

    SwitchToScene(sceneList[0]);

    effects = new Effects(projectionMatrix, viewMatrix);
    effects->Start(FADEIN, 0.5f);
}

int bullets_fired = 0;

void ProcessInput() {
    if (currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
        currentScene->state.player->movement = glm::vec3(0);
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        case SDL_MOUSEBUTTONDOWN:   // shoot with left mouse click
            if (bullets_fired < 12 && currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5]) {   // cannot shoot if there is no more ammunition, or we are on main menu
                if (!currentScene->state.projectiles[bullets_fired].isActive) {   // initialize bullet position/direction
                    currentScene->state.projectiles[bullets_fired].position = currentScene->state.player->position;

                    if (currentScene->state.player->animIndices == currentScene->state.player->animRight) { // if facing right, bullet goes right
                        currentScene->state.projectiles[bullets_fired].movement.x = 5.0f;
                        currentScene->state.projectiles[bullets_fired].acceleration = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                    else {  // if facing left, bullet goes left
                        currentScene->state.projectiles[bullets_fired].movement.x = -5.0f;
                        currentScene->state.projectiles[bullets_fired].acceleration = glm::vec3(-1.0f, 0.0f, 0.0f);
                    }

                }
                currentScene->state.projectiles[bullets_fired].isActive = true; // activate bullet
                Mix_PlayChannel(-1, laser, 0); // play laser sound effect
                ++bullets_fired;
            }
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                if (currentScene == sceneList[0]) {
                    currentScene->state.nextScene = 1;
                }
                else if (currentScene == sceneList[4] || currentScene == sceneList[5]) {
                    lives = 3;  // reset lives and ammo
                    bullets_fired = 0;
                    currentScene->state.nextScene = 0;
                }
                break;

            case SDLK_SPACE:    // jump occurs after SPACE is pressed, does not need to be held down
                if (currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
                    if (currentScene->state.player->collidedBottom) {
                        currentScene->state.player->jump = true;

                        Mix_PlayChannel(-1, sound_effect, 0); // play jump sound effect
                    }
                    break;
                }
            }
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5] && !gameIsOver) {
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }

        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
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
        if (currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
            if (currentScene->state.player->lives > lives) {    // keeps updated life count throughout the levels
                currentScene->state.player->lives = lives;
            }
        }

        effects->Update(FIXED_TIMESTEP);

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    if (currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5]) {
        viewMatrix = glm::mat4(1.0f);
        if (currentScene->state.player->position.x <= 5) { // if player is at beginning or end of map, keep screen locked

            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
        else if (currentScene->state.player->position.x >= 19) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-19, 3.75, 0));
        }
        else {  // if not follow player
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0)); // screen moves left as player moves right, 3.75 moves world up so we see from lower
        }

        viewMatrix = glm::translate(viewMatrix, effects->viewOffset);   // change camera according to offset
    }

    else {  // if main menu or endgame scene
        viewMatrix = glm::mat4(1.0f);   // reset viewmatrix
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (currentScene != sceneList[0] && currentScene != sceneList[4] && currentScene != sceneList[5]) { // life counter
        if (currentScene->state.player->lives == 3) {
            Util::DrawText(&program, Util::LoadTexture("font1.png"), "    Lives: 3", 0.4f, 0.0f, glm::vec3(0.5f, -0.35f, 0.0f));
        }
        else if (currentScene->state.player->lives == 2) {
            Util::DrawText(&program, Util::LoadTexture("font1.png"), "    Lives: 2", 0.4f, 0.0f, glm::vec3(0.5f, -0.35f, 0.0f));
        }
        else if (currentScene->state.player->lives == 1) {
            Util::DrawText(&program, Util::LoadTexture("font1.png"), "    Lives: 1", 0.4f, 0.0f, glm::vec3(0.5f, -0.35f, 0.0f));
        }
        else {
            Util::DrawText(&program, Util::LoadTexture("font1.png"), "    Lives: 0", 0.4f, 0.0f, glm::vec3(0.5f, -0.35f, 0.0f));
        }
    }
    program.SetViewMatrix(viewMatrix);  // keep rendering screen as player moves

    glUseProgram(program.programID);
    currentScene->Render(&program);     // only need this render

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

            bullets_fired = 0;  // reset bullet count
            if (currentScene->state.player) {
                lives = currentScene->state.player->lives;  // update life count
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