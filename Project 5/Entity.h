#pragma once
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
#include "Map.h"

enum EntityType {PLAYER, PLATFORM, ENEMY, PROJECTILE, ITEM, DOOR};
enum AIType {RUSHER, WAITANDGO, PATROLLER};
enum AIState {IDLE, RUNNING, PATROLLING};

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float speed;

    int lives = 3;

    float width = 1;
    float height = 1;

    bool jump = false;
    float jumpPower = 0;

    GLuint textureID;

    glm::mat4 modelMatrix;

    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    bool isActive = true;

    bool goLeft = true; // for AI Patroler
    bool defeatedPlayer = false;    // for AI, when true, player loses and game ends

    bool collided = false;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;

    bool landedOnAI = false;    // for player, when true, player has landed on an AI

    bool itemObtained = false;

    bool accessGranted = false;

    Entity();

    bool CheckCollision(Entity* other);

    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);

    void CheckCollisionsX(Map* map);    // keep old check collisions to check for collision with AI / other objects
    void CheckCollisionsY(Map* map);    // new functions handle collisions with map instead of platforms

    void CheckAICollision(Entity* object);

    void CheckProjectileCollision(Entity* objects, int objectCount, Map* map);

    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

    void AI(Entity* player);
    void AIRusher(Entity* player);
    void AIWaitAndGo(Entity* player);
    void AIPatroller();

    void Projectile(Entity* objects, int objectCount, Map* map);

    void Item(Entity* player);

    void Door(Entity* player);
};