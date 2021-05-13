#include "Level1.h"

#define LEVEL1_FLOOR_COUNT 8
#define LEVEL1_OBJECT_COUNT 90
#define LEVEL1_AI_COUNT 40

void Level1::Initialize(int carSelected) {
    state.nextScene = -1;

    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);

    std::vector<GLuint> textureIDs = { Util::LoadTexture("focus_texture.png") , Util::LoadTexture("focus_generic_black.png") ,Util::LoadTexture("focus_generic_blue.png") ,Util::LoadTexture("focus_generic_gray.png") ,Util::LoadTexture("focus_generic_green.png") ,Util::LoadTexture("focus_generic_white.png") ,
        Util::LoadTexture("evo_textureV2.png") , Util::LoadTexture("evo_textureV3.png"), Util::LoadTexture("evo_generic_black.png"), Util::LoadTexture("evo_generic_blue.png"), Util::LoadTexture("evo_generic_cream.png"), Util::LoadTexture("evo_generic_gray.png"), Util::LoadTexture("evo_generic_green.png"), Util::LoadTexture("evo_generic_red.png"), Util::LoadTexture("evo_generic_white.png") };
    // 15 textures, 0 - 5 focus, 6 - 14 evo/seat

    Mesh* focusMesh = new Mesh();
    focusMesh->LoadOBJ("focus.obj", 1);  // car all the same color

    Mesh* evoMesh = new Mesh();
    evoMesh->LoadOBJ("evo.obj", 1);  // car all the same color

    Mesh* seatMesh = new Mesh();
    seatMesh->LoadOBJ("seat.obj", 1);  // car all the same color

    Mesh* wheelMesh = new Mesh();
    wheelMesh->LoadOBJ("wheel.obj", 1);

    Mesh* focus_wheelMesh = new Mesh();
    focus_wheelMesh->LoadOBJ("wheel_focus.obj", 1);

    std::vector<Mesh*> meshes = { focusMesh , evoMesh , seatMesh };
    std::vector<Mesh*> wheel_meshes = { focus_wheelMesh, wheelMesh };

    int texture_index;

    state.player = new Entity();
    state.player->entityType = PLAYER;

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

    state.player->position = glm::vec3(1.5f, 0.5f, 0);
    state.player->rotation = glm::vec3(270, 0, 0);          // focus, evo, seat need to be rotated initially to be drawn correctly

    state.objects = new Entity[LEVEL1_OBJECT_COUNT];

    GLuint floorTextureID = Util::LoadTexture("tunnel_road.jpg");   // main road texture
    GLuint floor2TextureID = Util::LoadTexture("tunnel_road2.jpg"); // end of road texture
    GLuint sidewalkTextureID = Util::LoadTexture("sidewalk.jpg");   // sidewalk texture

    GLuint buildingTextureID = Util::LoadTexture("WindowsAndBalconies_S.jpg");
    GLuint building2TextureID = Util::LoadTexture("Balconies_S.jpg");
    GLuint building3TextureID = Util::LoadTexture("AlternatingWindows_S.jpg");
    GLuint building4TextureID = Util::LoadTexture("ApartmentWindows_S.jpg");
    GLuint building5TextureID = Util::LoadTexture("DarkGreyWindowsPaleBlocks_S.jpg");
    GLuint building6TextureID = Util::LoadTexture("BlueWindowsAndTiles_S.jpg"); 
    GLuint building7TextureID = Util::LoadTexture("GreyWindowsWithBlinds_S.jpg"); 
    GLuint building8TextureID = Util::LoadTexture("OfficeWindows_S.jpg");

    GLuint streetLightTextureID = Util::LoadTexture("diffuse_streetlamp.jpg");

    GLuint treeTextureID = Util::LoadTexture("Tree.png");

    std::vector<GLuint> buildingTextures = { buildingTextureID ,building2TextureID ,building3TextureID ,building4TextureID ,building5TextureID, building6TextureID, building7TextureID, building8TextureID };

    Mesh* floorMesh = new Mesh();
    floorMesh->LoadOBJ("cube.obj", 2);  // loads cube mesh for floor repeat twice

    Mesh* floor2Mesh = new Mesh();
    floor2Mesh->LoadOBJ("cube.obj", 1);  // loads cube mesh for floor once

    Mesh* streetLightMesh = new Mesh();
    streetLightMesh->LoadOBJ("streetlamp.obj", 1);

    Mesh* treeMesh = new Mesh();
    treeMesh->LoadOBJ("Tree.obj", 1);

    state.floor = new Entity[LEVEL1_FLOOR_COUNT];

    state.floor[0].textureID = floorTextureID;  // main road
    state.floor[0].mesh = floorMesh;
    state.floor[0].position = glm::vec3(0, 0, -90);    // road begins right behind the screen / starting point
    state.floor[0].rotation = glm::vec3(0, 0, 0);
    state.floor[0].acceleration = glm::vec3(0, 0, 0);
    state.floor[0].scale = glm::vec3(20, 0.5f, 200); // expand x (width) and z (length/depth) and shrink y to make it look flat
    state.floor[0].entityType = FLOOR;

    state.floor[1].textureID = floorTextureID;  // main road 2nd
    state.floor[1].mesh = floorMesh;
    state.floor[1].position = glm::vec3(0, 0, -290);    // next road obj is 200 units away (each road obj is 200 units long)
    state.floor[1].rotation = glm::vec3(0, 0, 0);
    state.floor[1].scale = glm::vec3(20, 0.5f, 200);
    state.floor[1].entityType = FLOOR;

    state.floor[2].textureID = sidewalkTextureID;    // sidewalk right side
    state.floor[2].mesh = floor2Mesh;
    state.floor[2].position = glm::vec3(15, 0.25, -90);
    state.floor[2].rotation = glm::vec3(0, 0, 0);
    state.floor[2].scale = glm::vec3(10, 0.5f, 200);
    state.floor[2].entityType = FLOOR;

    state.floor[3].textureID = sidewalkTextureID;     // sidewalk right side 2nd
    state.floor[3].mesh = floor2Mesh;
    state.floor[3].position = glm::vec3(15, 0.25, -290);
    state.floor[3].rotation = glm::vec3(0, 0, 0);
    state.floor[3].scale = glm::vec3(10, 0.5f, 280);
    state.floor[3].entityType = FLOOR;

    state.floor[4].textureID = sidewalkTextureID;     // sidewalk left side
    state.floor[4].mesh = floor2Mesh;
    state.floor[4].position = glm::vec3(-15, 0.25, -90);
    state.floor[4].rotation = glm::vec3(0, 0, 0);
    state.floor[4].scale = glm::vec3(10, 0.5f, 200);
    state.floor[4].entityType = FLOOR;

    state.floor[5].textureID = sidewalkTextureID;     // sidewalk left side 2nd
    state.floor[5].mesh = floor2Mesh;
    state.floor[5].position = glm::vec3(-15, 0.25, -290);
    state.floor[5].rotation = glm::vec3(0, 0, 0);
    state.floor[5].acceleration = glm::vec3(0, 0, 0);
    state.floor[5].scale = glm::vec3(10, 0.5f, 280);
    state.floor[5].entityType = FLOOR;

    state.floor[6].textureID = sidewalkTextureID;     // sidewalk end
    state.floor[6].mesh = floor2Mesh;
    state.floor[6].position = glm::vec3(0, 0.25, -510);
    state.floor[6].rotation = glm::vec3(0, 0, 0);
    state.floor[6].scale = glm::vec3(40, 0.5f, 200);
    state.floor[6].entityType = FLOOR;

    state.floor[7].textureID = floor2TextureID;     // corner street
    state.floor[7].mesh = floorMesh;
    state.floor[7].position = glm::vec3(0, 0, -490);
    state.floor[7].rotation = glm::vec3(0, 90, 0);
    state.floor[7].scale = glm::vec3(20, 0.5f, 200);
    state.floor[7].entityType = FLOOR;

    int j = 2;
    int k = 1;
    for (int i = 0; i < 61; ++i) { // all buildings = 60 objects
        state.objects[i].textureID = buildingTextures[i % buildingTextures.size()];
        state.objects[i].mesh = floor2Mesh;
        state.objects[i].rotation = glm::vec3(0, 0, 0);
        state.objects[i].scale = glm::vec3(10, rand() % 30 + 10, 15); // random heights of buildings

        if (i < 30) {   // right side buildings
            state.objects[i].position = glm::vec3(18, 0, -15 * i + 25);
        }
        else if (i < 58) {  // left side buildings
            state.objects[i].position = glm::vec3(-18, 0, -15 * j + 25);
            ++j;
        }
        else {  // end of road buildings
            state.objects[i].position = glm::vec3(-18 + 9 * k, 0, -15 * 30 + 25);
            ++k;
        }
    }

    j = 0;
    for (int i = 61; i < 76; ++i) { // 15 street light objects
        state.objects[i].textureID = streetLightTextureID;
        state.objects[i].mesh = streetLightMesh;
        state.objects[i].rotation = glm::vec3(0, 180 * (i % 2 + 1), 0);
        state.objects[i].scale = glm::vec3(1, 1.5f, 1);
        state.objects[i].position = glm::vec3(0, 0, -30 * j);
        ++j;
    }

    j = 0;
    for (int i = 76; i < 90; ++i) { // 14 tree objects
        state.objects[i].textureID = treeTextureID;
        state.objects[i].mesh = treeMesh;
        state.objects[i].rotation = glm::vec3(0, 180 * (i % 2 + 1), 0);
        state.objects[i].scale = glm::vec3(0.25, 0.5, 0.5);

        if (i % 2 == 0) {
            state.objects[i].position = glm::vec3(11, 0, -30 * j - 5);     // alternate between right and left side
        }
        else {
            state.objects[i].position = glm::vec3(-11, 0, -30 * j - 5);     // alternate between right and left side
        }
        ++j;
    }

    state.AI = new Entity[LEVEL1_AI_COUNT]; // initialize AI vehicles
    float x = -8.5f;
    for (int i = 0; i < LEVEL1_AI_COUNT; ++i) {

        x += 2.25;  // adjust x position so cars are in the middle of a lane
        if (x == -6.25f || x == 3.75) {
            x += 0.25;
        }
        else if (x == -1.5) {
            x += 0.75;
        }
        else if (x > 8.5) {
            x = -8.5;
        }

        state.AI[i].entityType = AI;

        if (i % 3 == 0) {   // AI vehicles are random, FOCUS car has specific mesh and textures
            state.AI[i].buildCar(meshes, textureIDs, wheel_meshes, 0, (rand() % 5) + 1);    // focus, texture must be index 1 - 5
        }
        else {  // EVO and SEAT cars have different meshes but share the same textures
            state.AI[i].buildCar(meshes, textureIDs, wheel_meshes, (i % 2) + 1, (rand() % 7) + 8);       // evo or seat, 1 = evo 2 = seat for mesh index, texture must be 8 - 14
        }

        state.AI[i].position = glm::vec3(x, 0.5, -rand() % 300 - 50);    // cars spawn randomly on road
        state.AI[i].rotation = glm::vec3(270, 0, 0);
        state.AI[i].speed = rand() % 5 + 2;   // cars have different speeds

        if (state.AI[i].position.x >= 0) {  // right side of road cars drive forward
            state.AI[i].velocity.z = -1.0f; 
        }
        else {  // oncoming traffic drive the opposite direction
            state.AI[i].rotation.z = 180;
            state.AI[i].velocity.z = 1.0f;
        }
    }
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);

    if (state.player->position.x >= 13 || state.player->position.x <= -13) {    // crash if drive into buildings
        state.player->crashed = true;
    }

    if (state.player->crashed) {
        state.nextScene = 5;    // if crashed, you lose
    }

    state.player->backLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
    state.player->backRightWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
    state.player->frontLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
    state.player->frontRightWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);

    if (state.player->position.z <= -390) {
        state.nextScene = 3;    // if reach the end, go to next level
    }

    for (int i = 0; i < LEVEL1_OBJECT_COUNT; ++i) {
        state.objects[i].Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
    }

    for (int i = 0; i < LEVEL1_FLOOR_COUNT; ++i) {
        state.floor[i].Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
    }

    for (int i = 0; i < LEVEL1_AI_COUNT; ++i) {
        state.AI[i].Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
        state.AI[i].backLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
        state.AI[i].backRightWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
        state.AI[i].frontLeftWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);
        state.AI[i].frontRightWheel->Update(deltaTime, state.player, state.objects, LEVEL1_OBJECT_COUNT, state.AI, LEVEL1_AI_COUNT);

        if (state.AI[i].position.z <= -400) {   // once cars reach the end of the road, they turn and drive back on the opposite side
            state.AI[i].rotation.z = 180;
            state.AI[i].velocity.z = 1.0;

            state.AI[i].position.x = -state.AI[i].position.x;
            //if (state.AI[i].position.x == 1.5) {    // cars drive back on the opposite lane
            //    state.AI[i].position.x = -1.5;
            //}
            //else if (state.AI[i].position.x == 3.75) {
            //    state.AI[i].position.x = -3.75;
            //}
            //else if (state.AI[i].position.x == 6.25) {
            //    state.AI[i].position.x = -6.25;
            //}
            //else if (state.AI[i].position.x == 8.5) {
            //    state.AI[i].position.x = -8.5;
            //}
        }

        else if (state.AI[i].position.z >= 10) {
            state.AI[i].position.z = 150;
            state.AI[i].rotation.z = 0;
            state.AI[i].velocity.z = -1.0;

            state.AI[i].position.x = -state.AI[i].position.x;

            //if (state.AI[i].position.x == -1.5) {
            //    state.AI[i].position.x = 1.5;
            //}
            //else if (state.AI[i].position.x == -3.75) {
            //    state.AI[i].position.x = 3.75;
            //}
            //else if (state.AI[i].position.x == -6.25) {
            //    state.AI[i].position.x = 6.25;
            //}
            //else if (state.AI[i].position.x == -8.5) {
            //    state.AI[i].position.x = 8.5;
            //}
        }
    }
}

void Level1::Render(ShaderProgram* program) {

    state.player->Render(program);

    state.player->backLeftWheel->Render(program);
    state.player->backRightWheel->Render(program);
    state.player->frontLeftWheel->Render(program);
    state.player->frontRightWheel->Render(program);

    for (int i = 0; i < LEVEL1_OBJECT_COUNT; ++i) {
        state.objects[i].Render(program);
    }

    for (int i = 0; i < LEVEL1_FLOOR_COUNT; ++i) {
        state.floor[i].Render(program);
    }

    for (int i = 0; i < LEVEL1_AI_COUNT; ++i) {
        state.AI[i].Render(program);
        state.AI[i].backLeftWheel->Render(program);
        state.AI[i].backRightWheel->Render(program);
        state.AI[i].frontLeftWheel->Render(program);
        state.AI[i].frontRightWheel->Render(program);
    }
}