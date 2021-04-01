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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define PLATFORM_COUNT 19
#define ENEMY_COUNT 3

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameIsOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

Mix_Music* music;   // music, can only play one mp3 file at a time
Mix_Chunk* sound_effect;    // sound effects, can have up to 16 in the mixer

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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  // initializes audio
    displayWindow = SDL_CreateWindow("Rise of the AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

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


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // gravity (Earth) always in effect
    state.player->speed = 1.75f;
    state.player->textureID = LoadTexture("george_0.png");

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

    state.platforms = new Entity[PLATFORM_COUNT];

    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");
    fontTextureID = LoadTexture("pixel_font.png");

    for (int i = 0; i < PLATFORM_COUNT; ++i) {  // floor    
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5.0f + i, -3.25f, 0.0f);
    }
    for (int i = 0; i + 11 < PLATFORM_COUNT - 2; ++i) { // left 2 platforms
        state.platforms[i + 11].entityType = PLATFORM;
        state.platforms[i + 11].textureID = platformTextureID;
        state.platforms[i + 11].position = glm::vec3(-5.0f + i, -0.25f, 0.0f);
    }
    for (int i = 0; i + 13 < PLATFORM_COUNT; ++i) { // right 2 platforms
        state.platforms[i + 13].entityType = PLATFORM;
        state.platforms[i + 13].textureID = platformTextureID;
        state.platforms[i + 13].position = glm::vec3(5.0f - i, -0.25f, 0.0f);
    }
    for (int i = 0; i + 15 < PLATFORM_COUNT; ++i) { // 4 middle platforms
        state.platforms[i + 15].entityType = PLATFORM;
        state.platforms[i + 15].textureID = platformTextureID;
        state.platforms[i + 15].position = glm::vec3(-1.5f + i, 2.0f, 0.0f);
    }

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Update(0, NULL, NULL, 0);   // needs to update only once so they are drawn at the correct spot
    }

    state.enemies = new Entity[ENEMY_COUNT];

    GLuint enemyTextureID = LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4, -2.25, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = PATROL;   // first AI patrols on the bottom level
    state.enemies[0].aiState = PATROLING;

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(0, 2.25, 0);
    state.enemies[1].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].speed = 0.75;
    state.enemies[1].aiType = WAITANDGO;    // second AI waits until player gets near then chases him in the middle
    state.enemies[1].aiState = IDLE;

    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(4.5, 0.25, 0);
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].speed = 0.5;
    state.enemies[2].aiType = RUSHER;   // third AI just runs at player
    state.enemies[2].aiState = RUNNING;

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

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:    // jump occurs after SPACE is pressed, does not need to be held down
                if (state.player->collidedBottom) { // only jumps if touching the ground
                    state.player->jump = true;  // tells game a jump is pending

                    Mix_PlayChannel(-1, sound_effect, 0); // play jump sound effect, -1 selects the first available channel, 0 plays sound once
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
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

    if (!gameIsOver) {  // only update if game is not over
        if (deltaTime < FIXED_TIMESTEP) {   // if delta time is quicker, accumulate extra time
            accumulator = deltaTime;
            return;
        }
        while (deltaTime >= FIXED_TIMESTEP) {   // if delta time is slower, keep updating so we continue at same pace
            // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
            state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);

            for (int i = 0; i < ENEMY_COUNT; ++i) {
                state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
                if (state.enemies[i].defeatedPlayer) {  // if any AI defeated player, game is over
                    gameIsOver = true;
                    break;
                }
            }

            if (!(state.enemies[0].isActive || state.enemies[1].isActive || state.enemies[2].isActive)) {   // if player defeats all AI game is over
                gameIsOver = true;
            }

            deltaTime -= FIXED_TIMESTEP;
        }
        accumulator = deltaTime;
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (gameIsOver) {
        if (state.enemies[0].defeatedPlayer || state.enemies[1].defeatedPlayer || state.enemies[2].defeatedPlayer) {    // player loses if any AI defeats them
            DrawText(&program, fontTextureID, "YOU LOSE!", 0.5f, 0.1f, glm::vec3(-2.25f, 0.75f, 0.0f));
        }
        else {  // if game is over, but no AI defeated player, player must have defeated all AI, player wins
            DrawText(&program, fontTextureID, "YOU WIN!", 0.5f, 0.1f, glm::vec3(-2.25f, 0.75f, 0.0f));
        }
    }

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; ++i) {
        state.enemies[i].Render(&program);
    }

    state.player->Render(&program);

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