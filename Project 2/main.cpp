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
#include <random>
#include <cstdlib>
#define PADDLE_COUNT 2

struct GameState {
    Entity* ball;
    Entity* paddles;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameIsOver = false;
int winner;

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
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glClearColor(0.0f, 1.0f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint ballTextureID = LoadTexture("soccer.png");
    fontTextureID = LoadTexture("pixel_font.png");

    float x = 1 / float(rand() % 1 + 1 - (-1)) ;
    float y = 1 / float(rand() % 3 + 1);

    state.ball = new Entity();
    state.ball->textureID = ballTextureID;
    state.ball->position = glm::vec3(0.0f, 0.0f, 0.0f);
    state.ball->movement = glm::vec3(1.0f, y, 0.0f);    // ball's random starting direction 
    state.ball->speed = 3.0f;

    state.paddles = new Entity[PADDLE_COUNT];

    state.paddles[0].position = glm::vec3(-5.0f, 0.0f, 0.0f);   // left paddle
    state.paddles[0].movement = glm::vec3(0);
    state.paddles[0].speed = 5.0f;

    state.paddles[1].position = glm::vec3(5.0f, 0.0f, 0.0f);    // right paddle
    state.paddles[1].movement = glm::vec3(0);
    state.paddles[1].speed = 5.0f;
}

void ProcessInput() {
    state.paddles[0].movement = glm::vec3(0);
    state.paddles[1].movement = glm::vec3(0);

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
            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_S] && state.paddles[0].position.y >= -3.25f) {    // if the key is pressed, paddle will move only if not already at top or bottom of screen
        state.paddles[0].movement.y = -1.0f;
    }
    else if (keys[SDL_SCANCODE_W] && state.paddles[0].position.y <= 3.25f) {
        state.paddles[0].movement.y = 1.0f;
    }
    if (keys[SDL_SCANCODE_DOWN] && state.paddles[1].position.y >= -3.25f) {
        state.paddles[1].movement.y = -1.0f;
    }
    else if (keys[SDL_SCANCODE_UP] && state.paddles[1].position.y <= 3.25f) {
        state.paddles[1].movement.y = 1.0f;
    }
    if (gameIsOver && keys[SDL_SCANCODE_SPACE]) {
        gameIsOver = false;

        float x = 1 / float(rand() % 3 + 1);
        float y = 1 / float(rand() % 3 + 1);

        state.paddles[0].movement = glm::vec3(0);
        state.paddles[1].movement = glm::vec3(0);

        state.paddles[0].position = glm::vec3(-5.0f, 0.0f, 0.0f);
        state.paddles[1].position = glm::vec3(5.0f, 0.0f, 0.0f);

        state.ball->position = glm::vec3(0);
        state.ball->movement = glm::vec3(-1.0f, y, 0.0f);
    }
}

float lastTicks = 0.0f;

void Update() { 

    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    if (!gameIsOver) {


        float paddle1x, paddle2x, paddle1y, paddle2y;

        for (int i = 0; i < PADDLE_COUNT; i++) {
            state.paddles[i].position += state.paddles[i].movement * state.paddles[i].speed * deltaTime;
            state.paddles[i].modelMatrix = glm::mat4(1.0f);
            state.paddles[i].modelMatrix = glm::translate(state.paddles[i].modelMatrix, state.paddles[i].position);
        }
        state.ball->position += state.ball->movement * state.ball->speed * deltaTime;

        paddle1x = state.paddles[0].position.x;
        paddle1y = state.paddles[0].position.y;
        paddle2x = state.paddles[1].position.x;
        paddle2y = state.paddles[1].position.y;

        // bounce off screen if ball hits top or bottom
        if (state.ball->position.y >= 3.25f) {
            state.ball->movement.y = -1.0f;
        }
        else if (state.ball->position.y <= -3.25f) {
            state.ball->movement.y = 1.0f;
        }

        // check collision w paddle 1
        if (((fabs(paddle1x - state.ball->position.x) - 1) < 0) && ((fabs(paddle1y - state.ball->position.y) - 1) < 0)) {
            state.ball->movement.x = 1.0f;
        }
        else if (((fabs(paddle2x - state.ball->position.x) - 1) < 0) && ((fabs(paddle2y - state.ball->position.y) - 1) < 0)) {    // check collision w paddle 2
            state.ball->movement.x = -1.0f;
        }

        if (state.ball->position.x <= -4.5f) {    // check if game over, player 2 wins
            winner = 1; // winner is 1 when player 2 is winner, 0 otherwise
            gameIsOver = true;

        }
        else if (state.ball->position.x >= 4.5f) {    // player 1 wins
            winner = 0;
            gameIsOver = true;

        }

        state.ball->modelMatrix = glm::mat4(1.0f);
        state.ball->modelMatrix = glm::translate(state.ball->modelMatrix, state.ball->position);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameIsOver) {
        DrawText(&program, fontTextureID, "GAME OVER!", 0.5f, 0.1f, glm::vec3(-2.5f, 1.0f, 0.0f));
        if (winner) {
            DrawText(&program, fontTextureID, "PLAYER 2 WINS!", 0.4f, 0.1f, glm::vec3(-3.0f, 0.0f, 0.0f));
        }
        else {
            DrawText(&program, fontTextureID, "PLAYER 1 WINS!", 0.4f, 0.1f, glm::vec3(-3.0f, 0.0f, 0.0f));
        }
        DrawText(&program, fontTextureID, "PRESS SPACE TO PLAY AGAIN!", 0.225f, 0.1f, glm::vec3(-4.0f, -1.5f, 0.0f));
    }
    
    for (int i = 0; i < PADDLE_COUNT; i++) {
        state.paddles[i].Render(&program);
    }
    state.ball->Render(&program);

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