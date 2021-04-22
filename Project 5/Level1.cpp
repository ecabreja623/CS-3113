#include "Level1.h"

#define LEVEL1_WIDTH 25
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 3

#define LEVEL1_AMMO_COUNT 12

unsigned int level1_data[] =
{
    45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
    45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
    45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
    45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
    45, 0, 0, 0, 0, 0, 0, 45, 45, 45, 45, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
    45, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 58, 58, 1, 0, 0, 0, 0, 0, 0, 45,
    45, 3, 3, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 1, 3, 0, 0, 3, 1, 1, 1, 1, 1, 1, 45,
    45, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 45,
};

void Level1::Initialize() {
    state.nextScene = -1;

    glClearColor(1.0f, 1.0f, 0.8f, 1.0f);

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 7);

    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // gravity (Earth) always in effect
    state.player->speed = 1.75f;
    state.player->textureID = Util::LoadTexture("george_0.png");

    state.player->lives = 3;


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

    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(9.0f, 0.75f, 0.0f);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;   // first AI patrols on the bottom level
    state.enemies[0].aiState = IDLE;

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(9.0f, -4.0f, 0.0f);
    state.enemies[1].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].speed = 0.75;
    state.enemies[1].aiType = WAITANDGO;    // second AI waits until player gets near then chases him in the middle
    state.enemies[1].aiState = IDLE;

    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(20.0f, 2.25f, 0.0f);
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].speed = 0.5;
    state.enemies[2].aiType = WAITANDGO;   // third AI just runs at player
    state.enemies[2].aiState = IDLE;

    state.projectiles = new Entity[LEVEL1_AMMO_COUNT];
    for (int i = 0; i < LEVEL1_AMMO_COUNT; ++i) {
        state.projectiles[i].entityType = PROJECTILE;
        state.projectiles[i].textureID = Util::LoadTexture("bullet.png");
        state.projectiles[i].width = 0.5f;
        state.projectiles[i].height = 0.2f;
        state.projectiles[i].speed = 10.0f;
        state.projectiles[i].isActive = false;
    }

    state.key = new Entity();
    state.key->entityType = ITEM;
    state.key->textureID = Util::LoadTexture("platformPack_item014.png");
    state.key->position = glm::vec3(9.0f, -0.75f, 0.0f);
    state.key->isActive = false;

    state.door = new Entity[2];
    state.door[0].entityType = DOOR;    // bottom half of door
    state.door[0].textureID = Util::LoadTexture("platformPack_tile059.png");
    state.door[0].position = glm::vec3(23.0f, -5.0f, 0.0f);

    state.door[1].entityType = DOOR;    // top half of door
    state.door[1].textureID = Util::LoadTexture("platformPack_tile050.png");
    state.door[1].position = glm::vec3(23.0f, -4.0f, 0.0f);
}

bool enemiesAliveL1 = true;

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    if (state.player->position.y <= -9.0f) {    // if player falls off map, lose 1 life
        --state.player->lives;
        state.player->position = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < LEVEL1_AMMO_COUNT; ++i) {
        state.projectiles[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {  // check if all enemies have been eliminated
        if (!state.enemies[i].isActive) {
            enemiesAliveL1 = false;
        }
        else {
            enemiesAliveL1 = true;
            break;
        }
    }

    if (!enemiesAliveL1) {    // if all enemies eliminated, spawn key for door
        if (!state.player->itemObtained) {
            state.key->isActive = true;
        }
    }

    state.key->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    state.door[0].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    state.door[1].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    if (state.player->itemObtained) {   // if key obtained, open door
        state.door[0].textureID = Util::LoadTexture("platformPack_tile057.png");
        state.door[1].textureID = Util::LoadTexture("platformPack_tile048.png");
    }

    if (state.player->position.x >= 23 && state.door[0].accessGranted) {    // if player enters door, go to next scene
        state.nextScene = 2;
    }
    else if (state.player->lives == 0) {
        state.nextScene = 5;
    }
}

void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);

    state.door[0].Render(program);
    state.door[1].Render(program);

    state.key->Render(program);

    state.player->Render(program);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; ++i) {
        state.enemies[i].Render(program);
    }
    for (int i = 0; i < LEVEL1_AMMO_COUNT; ++i) {
        state.projectiles[i].Render(program);
    }
}