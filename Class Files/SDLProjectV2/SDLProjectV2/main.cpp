#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>  // adds audio functions
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

//#define STB_IMAGE_IMPLEMENTATION      can remove these bc of util.h
//#include "stb_image.h"

#include "Util.h"
#include "Effects.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"

//#define PLATFORM_COUNT 12     // platforms replaced by maps

/*      moved to Level.h and Scene.h
#define ENEMY_COUNT 1


#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

unsigned int level1_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


struct GameState {
    Entity* player;
    //Entity* platforms;
    Map* map;
    Entity* enemies;
};

GameState state;
*/

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Mix_Music* music;   // music, can only play one mp3 file at a time
Mix_Chunk* sound_effect;    // sound effects, can have up to 16 in the mixer

/*      now in util.h
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
*/

Effects* effects;

Scene* currentScene;
//Level1* level1; REPLACE W SCENELIST
Scene* sceneList[2];

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  // initializes audio
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    //program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    //program.Load("shaders/vertex_textured.glsl", "shaders/effects_textured.glsl");  // can change shader file to change saturation, add grayscale, change darkness of reds, blues, greens, etc
    program.Load("shaders/vertex_lit.glsl", "shaders/fragment_lit.glsl");   // adds lighting, makes player a light source

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);  // cdq audio, 2 channels
    music = Mix_LoadMUS("dooblydoo.mp3");   // load mp3 file similar to loading texture
    Mix_PlayMusic(music, -1);               // play music, -1 means loop forever, 0 means play once, 1 means play twice
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);    // sets music volume to 1/4 of max volume

    sound_effect = Mix_LoadWAV("bounce.wav");   

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*      all moved to level1.cpp
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // gravity (Earth) always in effect
    state.player->speed = 1.75f;
    state.player->textureID = Util::LoadTexture("george_0.png");

    
    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;    // update height to make it not seem like its floating (depends on texture)
    state.player->width = 0.58f;    // update width so collisions are accuarte

    state.player->jumpPower = 5.0f;

    /*
    state.platforms = new Entity[PLATFORM_COUNT];

    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5.0f + i, -3.25f, 0.0f);
    }
    state.platforms[11].position = glm::vec3(0.0f, -2.25f, 0.0f);

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Update(0, NULL, NULL, 0);   // needs to update only once so they are drawn at the correct spot
    }
    */

    /*  also moved to level1.cpp
    state.enemies = new Entity[ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4, -2.25, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
    */

    //GLuint mapTextureID = Util::LoadTexture("tileset.png"); NOW IN LEVEL1.CPP
    //state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);    // 1x4  tiles, 1.0f = tile size NOW IN LEVEL1.CPP

    //level1 = new Level1();  // initializes level 1 and switches to it
    //SwitchToScene(level1);

    sceneList[0] = new Level1();
    sceneList[1] = new Level2();
    SwitchToScene(sceneList[0]);

    effects = new Effects(projectionMatrix, viewMatrix);
    //effects->Start(SHRINK, 5.0f);
}

void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);    // will add currentScene-> everywhere where state->... is

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:    // jump occurs after SPACE is pressed, does not need to be held down
                if (currentScene->state.player->collidedBottom) { // only jumps if touching the ground
                    currentScene->state.player->jump = true;  // tells game a jump is pending
                    
                    currentScene->state.player->currentAnim = JUMPING;
                    
   

                    Mix_PlayChannel(-1, sound_effect, 0); // play jump sound effect, -1 selects the first available channel, 0 plays sound once
                }
                else{
                    currentScene->state.player->currentAnim = FALLING;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        //currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        currentScene->state.player->currentAnim = RUNNING;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        //currentScene->state.player->animIndices = currentScene->state.player->animRight;
        currentScene->state.player->currentAnim = RUNNING;
    }
    else {
        currentScene->state.player->currentAnim = IDLEING;
    }

    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
}

#define FIXED_TIMESTEP 0.0166666f   // 60 fps, 1 frame every 0.1666 sec
float lastTicks = 0;
float accumulator = 0.0f;

bool lastCollidedBottom = false;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;

    if (deltaTime < FIXED_TIMESTEP) {   // if delta time is quicker, accumulate extra time
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {   // if delta time is slower, keep updating so we continue at same pace
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        //state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);

        //state.player->Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.map);
        currentScene->Update(FIXED_TIMESTEP);

        program.SetLightPosition(currentScene->state.player->position);  // makes player light source

        effects->Update(FIXED_TIMESTEP);

        /*
        for (int i = 0; i < ENEMY_COUNT; ++i) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        }
        */

        if (lastCollidedBottom == false && currentScene->state.player->collidedBottom) {
            //effects->Start(SHAKE, 2.0f);    // shakes every time the player lands on the ground
        }
        lastCollidedBottom = currentScene->state.player->collidedBottom;

        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) { // if player is past position 5, follow player
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0)); // screen moves left as player moves right, 3.75 moves world up so we see from lower
    }
    else {  // if not then keep screen locked
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }    

    viewMatrix = glm::translate(viewMatrix, effects->viewOffset);   // change camera according to offset
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);  // keep rendering screen as player moves

    //state.map->Render(&program);

    /*
    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Render(&program);
    }
    */

    /*
    for (int i = 0; i < ENEMY_COUNT; ++i) {
        state.enemies[i].Render(&program);
    }
    */

    //state.player->Render(&program);

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

        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);

        Render();
    }

    Shutdown();
    return 0;
}