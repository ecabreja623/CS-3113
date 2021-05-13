#include "EndGameWin.h"

void EndGameWin::Initialize(int carSelected) {
	state.nextScene = -1;
    glClearColor(0.3f, 0.0f, 0.0f, 1.0f);

    // textures held in a vector
    std::vector<GLuint> textureIDs = { Util::LoadTexture("focus_texture.png") , Util::LoadTexture("evo_textureV2.png") , Util::LoadTexture("evo_textureV3.png") };

    // car and wheel meshes
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
    state.player->entityType = PLAYER;

    state.carSelected = carSelected;

    // mesh index and texture index same as carSelected
    state.player->buildCar(meshes, textureIDs, wheel_meshes, state.carSelected, state.carSelected); 

    state.player->position = glm::vec3(0, -1, -5);
    state.player->rotation = glm::vec3(270, 0, 0);

}

void EndGameWin::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, NULL, 0, NULL, 0);

    state.player->rotation.z -= 0.75f;

    state.player->backLeftWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
    state.player->backRightWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
    state.player->frontLeftWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
    state.player->frontRightWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
}

void EndGameWin::Render(ShaderProgram* program) {

    state.player->Render(program);

    state.player->backLeftWheel->Render(program);
    state.player->backRightWheel->Render(program);
    state.player->frontLeftWheel->Render(program);
    state.player->frontRightWheel->Render(program);
}