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

#include "Entity.h"
#define ENEMY_COUNT 3

struct GameState {
    Entity* player;
    Entity* enemies;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

//float player_x = 0;
//float player_rotate = 0;

// all in entity
//glm::vec3 player_position = glm::vec3(0, 0, 0);   
//glm::vec3 player_movement = glm::vec3(0, 0, 0);
//float player_speed = 1.0f;  // 1 unit per second

//GLuint playerTextureID;

/*  moved to init
// pointer to array of tex atlas indices (which sprites we gonna use)
int* animRight = new int[4]{ 3, 7, 11, 15 };
int* animLeft = new int[4]{ 1, 5, 9, 13 };
int* animUp = new int[4]{ 2, 6, 10, 14 };
int* animDown = new int[4]{ 0, 4, 8, 12 };

// control current animation
int *animIndices = animRight;  
int animFrames = 4; // how many sprites/frames
int animIndex = 0;  // starting at first sprite of animIndices
float animTime = 0; // keeps track of when to change sprite/texture for animation
*/

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

/* moved to header file and cpp
void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index){   // picks sprite from atlas of sprites

    int cols = 4;   // CHANGE DEPENDING ON ROWS/COLUMNS ON YOUR TEXTURE ATLAS
    int rows = 4;

    float u = (float)(index % cols) / (float)cols;  // UV coords, from 0 to 1
    float v = (float)(index / cols) / (float)rows;

    float width = 1.0f / (float)cols;   // width and height of each sprite w evenly spaced atlas
    float height = 1.0f / (float)rows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v };
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
*/

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

    //playerTextureID = LoadTexture("r34gtr.png");    // loads the image onto video card
    //playerTextureID = LoadTexture("george_0.png");

    state.player = new Entity();
    state.player->position = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 1.0f;
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

    state.enemies = new Entity[ENEMY_COUNT];

    GLuint enemyTextureID = LoadTexture("ctg.png");

    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(-2.0f, -2.0f, 0.0f);

    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(0.0f, -2.0f, 0.0f);

    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(2.0f, -2.0f, 0.0f);
}

void ProcessInput() {

    //player_movement = glm::vec3(0); 
    state.player->movement = glm::vec3(0);  // if nothing is pressed, don't move

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:     // if game quit or window closes, end game
            gameIsRunning = false;
            break;
        case SDL_KEYDOWN:       // 
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                //player_movement.x = -1.0f; // moves left but very slowly



                break;
            case SDLK_RIGHT:
                // Move the player right
                //player_movement.x = 1.0f;   // moves right but very slowly



                break;
            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT]) {  // if key is held down
        //PlayerLeft();
        //player_movement.x = -1.0f;  
        state.player->movement.x = -1.0f;   // moves left much quicker and smoother than SDLK
        state.player->animIndices = state.player->animLeft;     // assigns appropriate animation to which direction we are moving
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {    // can't move left and right, but can move left up or left down
        //PlayerRight();
        //player_movement.x = 1.0f;
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    if (keys[SDL_SCANCODE_DOWN]) {      // can be changed to W,A,S,D instead of Left, right, etc
        //PlayerLeft();
        //player_movement.y = -1.0f;
        state.player->movement.y = -1.0f;
        state.player->animIndices = state.player->animDown;
    }
    else if (keys[SDL_SCANCODE_UP]) {
        //PlayerRight();
        //player_movement.y = 1.0f;
        state.player->movement.y = 1.0f;
        state.player->animIndices = state.player->animUp;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);  // makes it so you will not move faster by holding multiple keys downs
    }
}

float lastTicks = 0.0f;

void Update() { 

    float ticks = (float)SDL_GetTicks() / 1000.0f;  // seconds since initialization
    float deltaTime = ticks - lastTicks;    // subtracts time since init. from last frame to get deltatime
    lastTicks = ticks;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime);
    }
    state.player->Update(deltaTime);    // replaces all of the previous code with the code in Entity.cpp, cleans up main.cpp

    /*  moved to Entity.cpp
    if (glm::length(player_movement) != 0) {    // only animate if moving
        animTime += deltaTime;

        if (animTime >= 0.25f) { //change sprite/texture after a certain amount of time (0.25 sec), lower time = faster animation

            animTime = 0.0f;    // resets time
            animIndex++;        // changes sprite/texture
            if (animIndex >= animFrames)    // loops back to first sprite of animation
            {
                animIndex = 0;
            }
        }
    }
    else {
        animIndex = 0;  // if not moving, stand still (do not stop in sprite that looks like its moving)
    }
    */

    //player_x += 1.0f;   // adds 1 to x position every time it updates
    //player_x += 1.0f * deltaTime;   // moves 1 unit per second
    //player_rotate += -90.0f * deltaTime;    //positive = counter clockwise, negative = clockwise rotation
    //player_position += player_movement * player_speed * deltaTime;


    //modelMatrix = glm::mat4(1.0f);      //matrix is identity matrix, centers triangle every frame if not commented out
    //modelMatrix = glm::translate(modelMatrix, player_position);     // move to new position

    //modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.0f, 0.0f));     // move triangle, vec3 is x y z, z used for rotation and 3d
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));    // rotates by certain degree, positive z value = counter clockwise, negative = clockwise rotation
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));   // scale triangle by scalar amount
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    //program.SetModelMatrix(modelMatrix);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }
    state.player->Render(&program); // cleans up main.cpp, does all that is commented out

    //DrawSpriteFromTextureAtlas(&program, playerTextureID, 7);   // draw sprite at index 7 from atlas
    //DrawSpriteFromTextureAtlas(&program, playerTextureID, animIndices[animIndex]);  // draws sprite at whatever index is chosen

    /*  commented out bc ^ does all this in the function
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
    */

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