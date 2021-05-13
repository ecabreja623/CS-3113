#include "Level2.h"

#define LEVEL2_FLOOR_COUNT 3
#define LEVEL2_OBJECT_COUNT 174
#define LEVEL2_AI_COUNT 50

void Level2::Initialize(int carSelected) {
    state.nextScene = -1;

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

    std::vector<GLuint> textureIDs = { Util::LoadTexture("focus_texture.png") , Util::LoadTexture("focus_generic_black.png") ,Util::LoadTexture("focus_generic_blue.png") ,Util::LoadTexture("focus_generic_gray.png") ,Util::LoadTexture("focus_generic_green.png") ,Util::LoadTexture("focus_generic_white.png") ,
        Util::LoadTexture("evo_textureV2.png") , Util::LoadTexture("evo_textureV3.png"), Util::LoadTexture("evo_generic_black.png"), Util::LoadTexture("evo_generic_blue.png"), Util::LoadTexture("evo_generic_cream.png"), Util::LoadTexture("evo_generic_gray.png"), Util::LoadTexture("evo_generic_green.png"), Util::LoadTexture("evo_generic_red.png"), Util::LoadTexture("evo_generic_white.png") };
    // 15 textures, 0 - 5 focus, 6 - 14 evo/seat

    Mesh* focusMesh = new Mesh();
    focusMesh->LoadOBJ("focus.obj", 1);

    Mesh* evoMesh = new Mesh();
    evoMesh->LoadOBJ("evo.obj", 1);

    Mesh* seatMesh = new Mesh();
    seatMesh->LoadOBJ("seat.obj", 1);

    Mesh* wheelMesh = new Mesh();
    wheelMesh->LoadOBJ("wheel.obj", 1);

    Mesh* focus_wheelMesh = new Mesh();
    focus_wheelMesh->LoadOBJ("wheel_focus.obj", 1);

    std::vector<Mesh*> meshes = { focusMesh , evoMesh , seatMesh };
    std::vector<Mesh*> wheel_meshes = { focus_wheelMesh, wheelMesh };

    int texture_index;

    state.player = new Entity();
    state.player->entityType = PLAYER;  // carType = SEAT, FOCUS, EVO

    state.carSelected = carSelected;

    if (state.carSelected == 0) {
        texture_index = 0;  // focus player texture
    }
    else if (state.carSelected == 1) {
        texture_index = 6;  // evo player texture
    }
    else {
        texture_index = 7;  // seat player texture
    }

    state.player->buildCar(meshes, textureIDs, wheel_meshes, state.carSelected, texture_index);

    state.player->position = glm::vec3(2.5f, 0.5f, 0);
    state.player->rotation = glm::vec3(270, 0, 0);

    state.objects = new Entity[LEVEL2_OBJECT_COUNT];

    GLuint floorTextureID = Util::LoadTexture("road.jpg");  // main road texture

    GLuint barrierTextureID = Util::LoadTexture("RoadBlockade.png");

    GLuint buildingTextureID = Util::LoadTexture("WindowsAndBalconies_S.jpg");
    GLuint building2TextureID = Util::LoadTexture("Balconies_S.jpg");
    GLuint building3TextureID = Util::LoadTexture("AlternatingWindows_S.jpg");
    GLuint building4TextureID = Util::LoadTexture("ApartmentWindows_S.jpg");
    GLuint building5TextureID = Util::LoadTexture("DarkGreyWindowsPaleBlocks_S.jpg");
    GLuint building6TextureID = Util::LoadTexture("BlueWindowsAndTiles_S.jpg");
    GLuint building7TextureID = Util::LoadTexture("GreyWindowsWithBlinds_S.jpg");
    GLuint building8TextureID = Util::LoadTexture("OfficeWindows_S.jpg");

    std::vector<GLuint> buildingTextures = { buildingTextureID ,building2TextureID ,building3TextureID ,building4TextureID ,building5TextureID, building6TextureID, building7TextureID, building8TextureID };

    GLuint streetLightTextureID = Util::LoadTexture("diffuse_streetlamp.jpg");

    Mesh* floorMesh = new Mesh();
    floorMesh->LoadOBJ("cube.obj", 1);

    Mesh* streetLightMesh = new Mesh();
    streetLightMesh->LoadOBJ("streetlamp.obj", 1);

    Mesh* barrierMesh = new Mesh();
    barrierMesh->LoadOBJ("RoadBlockade_02.obj", 1);

    state.floor = new Entity[LEVEL2_FLOOR_COUNT];

    state.floor[0].textureID = floorTextureID;
    state.floor[0].mesh = floorMesh;
    state.floor[0].position = glm::vec3(0, 0, -90);    // road begins right behind the screen / starting point
    state.floor[0].rotation = glm::vec3(0, 90, 0);
    state.floor[0].scale = glm::vec3(20, 0.5f, 200);
    state.floor[0].entityType = FLOOR;

    state.floor[1].textureID = floorTextureID;
    state.floor[1].mesh = floorMesh;
    state.floor[1].position = glm::vec3(0, 0, -290);    // next road obj is 200 units away (each road obj is 200 units long)
    state.floor[1].rotation = glm::vec3(0, 90, 0);
    state.floor[1].scale = glm::vec3(20, 0.5f, 200);
    state.floor[1].entityType = FLOOR;

    state.floor[2].textureID = floorTextureID;
    state.floor[2].mesh = floorMesh;
    state.floor[2].position = glm::vec3(0, 0, -490);
    state.floor[2].rotation = glm::vec3(0, 90, 0);
    state.floor[2].scale = glm::vec3(20, 0.5f, 200);
    state.floor[2].entityType = FLOOR;

    int j = 0;
    int k = 0;
    for (int i = 0; i < 75; ++i) { // 74 barrier objects
        state.objects[i].textureID = barrierTextureID;
        state.objects[i].mesh = barrierMesh;
        state.objects[i].rotation = glm::vec3(0, 90, 0);
        state.objects[i].scale = glm::vec3(1.0f, 1.5f, 9.0f);   // length of barrier = 24 units, 25 barriers each, 75 total

        if (i < 25) {
            state.objects[i].position = glm::vec3(0, 0, (-24 * i) + 8);  // middle concrete barriers
        }
        else if (i < 50) {
            state.objects[i].position = glm::vec3(10, 0, (-24 * j) + 8); // right concrete barriers
            ++j;
        }
        else {
            state.objects[i].position = glm::vec3(-10, 0, (-24 * k) + 8); // left concrete barriers
            ++k;
        }
    }

    j = 0;
    for (int i = 75; i < 95; ++i) {   // 19 street light objects
        state.objects[i].textureID = streetLightTextureID;  // street lights
        state.objects[i].mesh = streetLightMesh;
        state.objects[i].rotation = glm::vec3(0, 180 * (i % 2 + 1), 0);
        state.objects[i].scale = glm::vec3(1, 1.5f, 1);
        state.objects[i].position = glm::vec3(0, 0, -30 * j);
        ++j;
    }

    j = 2;
    k = 2;
    for (int i = 95; i < LEVEL2_OBJECT_COUNT - 1; ++i) { // all buildings = 78 objects
        state.objects[i].textureID = buildingTextures[i % buildingTextures.size()];
        state.objects[i].mesh = floorMesh;
        state.objects[i].rotation = glm::vec3(0, 0, 0);
        state.objects[i].scale = glm::vec3(10, rand() % 50 + 20, 15); // buildings range in size (height)

        if (i < 134) {
            state.objects[i].position = glm::vec3(rand() % 5 + 23, 5, -15 * j + 25);    // right side buildings (range in position to look more natural)
            ++j;
        }
        else {
            state.objects[i].position = glm::vec3(-rand() % 5 - 23, 5, -15 * k + 25);    // left side buildings (range in position to look more natural)
            ++k;
        }
    }

    state.objects[173].textureID = Util::LoadTexture("skyline.png");    // skyline horizon at end of road
    state.objects[173].billboard = true;
    state.objects[173].scale = glm::vec3(170, 100, 1);
    state.objects[173].position = glm::vec3(0, 35, -670);

    state.AI = new Entity[LEVEL2_AI_COUNT];
    float x = -12.5f;
    for (int i = 0; i < LEVEL2_AI_COUNT; ++i) {

        x += 5;
        if (x > 7.5) {    // cars stay in lane
            x = -7.5;
        }

        state.AI[i].entityType = AI;

        if (i % 3 == 0) {   // random vehicles
            state.AI[i].buildCar(meshes, textureIDs, wheel_meshes, 0, (rand() % 5) + 1); // focus, texture must be index 1 - 5
        }
        else {
            state.AI[i].buildCar(meshes, textureIDs, wheel_meshes, (i % 2) + 1, (rand() % 7) + 8);        // evo or seat, 1 = evo 2 = seat for mesh index, texture must be 8 - 14
        }

        state.AI[i].position = glm::vec3(x, 0.5, -rand() % 520 - 10);
        state.AI[i].rotation = glm::vec3(270, 0, 0);
        state.AI[i].speed = rand() % 5 + 3;

        if (state.AI[i].position.x >= 0) {
            state.AI[i].velocity.z = -1.0f; // make oncoming traffic drive the opposite direction
        }
        else {
            state.AI[i].rotation.z = 180;
            state.AI[i].velocity.z = 1.0f;
        }
    }
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);

    if (state.player->position.x <= 0 || state.player->position.x >= 10) {  // crash if run into barriers
        state.player->crashed = true;
    }

    if (state.player->crashed) {
        state.nextScene = 5;    // if crashed, you lose
    }

    state.player->backLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
    state.player->backRightWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
    state.player->frontLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
    state.player->frontRightWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);

    if (state.player->position.z <= -540) {
        state.nextScene = 4;    // if reach the end, you win
    }

    for (int i = 0; i < LEVEL2_OBJECT_COUNT; ++i) {
        state.objects[i].Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
    }

    for (int i = 0; i < LEVEL2_FLOOR_COUNT; ++i) {
        state.floor[i].Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
    }

    for (int i = 0; i < LEVEL2_AI_COUNT; ++i) {
        state.AI[i].Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
        state.AI[i].backLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
        state.AI[i].backRightWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
        state.AI[i].frontLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
        state.AI[i].frontRightWheel->Update(deltaTime, state.player, state.objects, LEVEL2_OBJECT_COUNT, state.AI, LEVEL2_AI_COUNT);
        
        if (state.AI[i].position.z <= -590) {   // cars drive on opposite lane if they reach end of the road
            state.AI[i].rotation.z = 180;
            state.AI[i].velocity.z = 1.0;

            if (state.AI[i].position.x == 2.5) {
                state.AI[i].position.x = -2.5;
            }
            else if (state.AI[i].position.x == 7.5) {
                state.AI[i].position.x = -7.5;
            }
        }
        else if (state.AI[i].position.z >= 10) {
            state.AI[i].position.z = 150;
            state.AI[i].rotation.z = 0;
            state.AI[i].velocity.z = -1.0;

            if (state.AI[i].position.x == -2.5) {
                state.AI[i].position.x = 2.5;
            }
            else if (state.AI[i].position.x == -7.5) {
                state.AI[i].position.x = 7.5;
            }
        }

    }
}

void Level2::Render(ShaderProgram* program) {

    state.player->Render(program);

    state.player->backLeftWheel->Render(program);
    state.player->backRightWheel->Render(program);
    state.player->frontLeftWheel->Render(program);
    state.player->frontRightWheel->Render(program);

    for (int i = 0; i < LEVEL2_OBJECT_COUNT; ++i) {
        state.objects[i].Render(program);
    }

    for (int i = 0; i < LEVEL2_FLOOR_COUNT; ++i) {
        state.floor[i].Render(program);
    }

    for (int i = 0; i < LEVEL2_AI_COUNT; ++i) {
        state.AI[i].Render(program);
        state.AI[i].backLeftWheel->Render(program);
        state.AI[i].backRightWheel->Render(program);
        state.AI[i].frontLeftWheel->Render(program);
        state.AI[i].frontRightWheel->Render(program);
    }
}