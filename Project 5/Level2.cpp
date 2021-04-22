#include "Level2.h"

#define LEVEL2_WIDTH 25
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 2

#define LEVEL2_AMMO_COUNT 12

unsigned int level2_data[] =
{
    60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60,
    60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60,
    60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 60, 60, 0, 0, 0, 0, 60, 60,
    60, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60,
    60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 60, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60,
    17, 17, 17, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60,
    17, 17, 17, 17, 5, 5, 5, 17, 17, 17, 17, 0, 0, 0, 0, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 19, 19, 19, 17, 17, 17, 17, 70, 70, 70, 70, 70, 17, 17, 17, 17, 17, 17, 17, 17, 17,
};

void Level2::Initialize() {
    state.nextScene = -1;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 14, 7);

    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, 0, 0);
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

    state.player->height = 0.8f;
    state.player->width = 0.58f;

    state.player->jumpPower = 5.0f;

    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(8.5f, -1.0f, 0.0f);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(22.9f, 0.0f, 0.0f);
    state.enemies[1].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;

    state.projectiles = new Entity[LEVEL2_AMMO_COUNT];
    for (int i = 0; i < LEVEL2_AMMO_COUNT; ++i) {
        state.projectiles[i].entityType = PROJECTILE;
        state.projectiles[i].textureID = Util::LoadTexture("bullet.png");
        state.projectiles[i].width = 0.5f;
        state.projectiles[i].height = 0.2f;
        state.projectiles[i].speed = 10.0f;
        state.projectiles[i].isActive = false;
    }

    state.key = new Entity();
    state.key->entityType = ITEM;
    state.key->textureID = Util::LoadTexture("platformPack_item016.png");
    state.key->position = glm::vec3(17.0f, -0.75f, 0.0f);
    state.key->isActive = false;

    state.door = new Entity[2];
    state.door[0].entityType = DOOR;    // bottom half of door
    state.door[0].textureID = Util::LoadTexture("platformPack_tile061.png");
    state.door[0].position = glm::vec3(23.0f, -5.0f, 0.0f);

    state.door[1].entityType = DOOR;    // top half of door
    state.door[1].textureID = Util::LoadTexture("platformPack_tile052.png");
    state.door[1].position = glm::vec3(23.0f, -4.0f, 0.0f);
}

bool enemiesAliveL2 = true;

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    if (state.player->position.y < -5.0f 
        && (state.player->position.x >= 4.0f && state.player->position.x <= 6.0f) 
        && state.player->collidedBottom) 
    { // if lands in lava

        --state.player->lives;
        state.player->position = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    if (state.player->position.y < -4.0f 
        && (state.player->position.x >= 11.0f && state.player->position.x <= 15.0f) 
        && state.player->collidedBottom) 
    { // if lands on spikes

        --state.player->lives;
        state.player->position = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; ++i) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < LEVEL2_AMMO_COUNT; ++i) {
        state.projectiles[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; ++i) {  // check if all enemies have been eliminated
        if (!state.enemies[i].isActive) {
            enemiesAliveL2 = false;
        }
        else {
            enemiesAliveL2 = true;
            break;
        }
    }

    if (!enemiesAliveL2) {    // if all enemies eliminated, spawn key for door
        if (!state.player->itemObtained) {
            state.key->isActive = true;
        }
    }

    state.key->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    state.door[0].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    state.door[1].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    if (state.player->itemObtained) {   // if key is obtained, open door
        state.door[0].textureID = Util::LoadTexture("platformPack_tile057.png");
        state.door[1].textureID = Util::LoadTexture("platformPack_tile048.png");
    }

    if (state.player->position.x >= 23 && state.door[0].accessGranted) {    // if player enters door, go to next scene
        state.nextScene = 3;
    }
    else if (state.player->lives == 0) {
        state.nextScene = 5;
    }
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);

    state.door[0].Render(program);
    state.door[1].Render(program);

    state.key->Render(program);

    state.player->Render(program);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; ++i) {
        state.enemies[i].Render(program);
    }
    for (int i = 0; i < LEVEL2_AMMO_COUNT; ++i) {
        state.projectiles[i].Render(program);
    }
}