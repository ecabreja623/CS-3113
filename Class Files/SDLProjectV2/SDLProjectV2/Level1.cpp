#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

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

void Level1::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.

    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);   // gravity (Earth) always in effect
    state.player->speed = 1.75f;
    //state.player->textureID = Util::LoadTexture("george_0.png");

    
 

    GLuint IdleTexture = Util::LoadTexture("Idle.png");
    GLuint RunningTexture = Util::LoadTexture("Run.png");
    GLuint JumpingTexture = Util::LoadTexture("Jump.png");
    GLuint FallingTexture = Util::LoadTexture("Fall.png");
    GLuint AttackingTexture = Util::LoadTexture("Attack2.png");
    GLuint DyingTexture = Util::LoadTexture("Death.png");

    std::vector<GLuint> textureIDlist{ IdleTexture, RunningTexture, JumpingTexture, FallingTexture, AttackingTexture, DyingTexture };  
    // idle (6), run (8), jump (2), falling (2), attacking (8), dying (7)

    state.player->textureID = IdleTexture;

    state.player->animIndices = new int[6]{ 0,1,2,3,4,5 };
    state.player->animFrames = 6;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 6;
    state.player->animRows = 1;

    /*
    switch (state.player->currentAnim) {
    case IDLEING:
        state.player->textureID = IdleTexture;

        state.player->animIndices = new int[6]{ 0,1,2,3,4,5};
        state.player->animFrames = 6;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 6;
        state.player->animRows = 1;
        
        break;

    case RUNNING:
        state.player->textureID = RunningTexture;

        state.player->animIndices = new int[8]{ 0,1,2,3,4,5,6,7 };
        state.player->animFrames = 8;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 8;
        state.player->animRows = 1;

        break;

    case JUMPING:
        state.player->textureID = JumpingTexture;

        state.player->animIndices = new int[2]{ 0,1 };
        state.player->animFrames = 2;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 2;
        state.player->animRows = 1;

        break;

    case FALLING:
        state.player->textureID = FallingTexture;

        state.player->animIndices = new int[2]{ 0,1 };
        state.player->animFrames = 2;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 2;
        state.player->animRows = 1;

        break;

    case SHOOTING:
        state.player->textureID = AttackingTexture;

        state.player->animIndices = new int[8]{ 0,1,2,3,4,5,6,7 };
        state.player->animFrames = 8;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 8;
        state.player->animRows = 1;

        break;

    case DYING:
        state.player->textureID = DyingTexture;

        state.player->animIndices = new int[7]{ 0,1,2,3,4,5,6 };
        state.player->animFrames = 7;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 7;
        state.player->animRows = 1;

        break;
    }
    */
    //state.player->textureID = Util::LoadTexture("Idle.png");

    /*
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
    */

    //state.player->height = 0.8f;    // update height to make it not seem like its floating (depends on texture)
    //state.player->width = 0.58f;    // update width so collisions are accuarte
    state.player->height = 1.0f;
    state.player->width = 1.0f;

    state.player->jumpPower = 5.0f;

    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];

    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4, -2.25, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
}
void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);

    if (state.player->position.x >= 12) {
        state.nextScene = 1;
    }
}
void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}