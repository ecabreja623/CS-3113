#include "VehicleSelection.h"

void VehicleSelection::Initialize(int carSelected) {
	state.nextScene = -1;

	glClearColor(0.3f, 0.0f, 0.6f, 1.0f);

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

    state.AI = new Entity[3];

    // display the three available cars
    state.AI[0].entityType = PLAYER;
    state.AI[0].carType = FOCUS;
    state.AI[0].position = glm::vec3(-2.5, -1.5f, -10);
    state.AI[0].rotation = glm::vec3(270, 0, 0);
    state.AI[0].buildCar(meshes, textureIDs, wheel_meshes, 0, 0);

    state.AI[1].entityType = PLAYER;
    state.AI[1].carType = EVO;
    state.AI[1].position = glm::vec3(0.5, -1.5f, -10);
    state.AI[1].rotation = glm::vec3(270, 0, 0);
    state.AI[1].buildCar(meshes, textureIDs, wheel_meshes, 1, 1);

    state.AI[2].entityType = PLAYER;
    state.AI[2].carType = SEAT;
    state.AI[2].position = glm::vec3(3.5, -1.5f, -10);
    state.AI[2].rotation = glm::vec3(270, 0, 0);
    state.AI[2].buildCar(meshes, textureIDs, wheel_meshes, 2, 2);

    state.carSelected = 0;  // starts with the first vehicle, can change selection with arrow keys
}

void VehicleSelection::Update(float deltaTime) {
    for (int i = 0; i < 3; ++i) {
        state.AI[i].Update(deltaTime, state.player, NULL, 0, NULL, 0);
        state.AI[i].backLeftWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
        state.AI[i].backRightWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
        state.AI[i].frontLeftWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);
        state.AI[i].frontRightWheel->Update(deltaTime, state.player, NULL, 0, NULL, 0);

        if (state.carSelected == i) {   // currently selected car is displayed closer and rotates
            state.AI[i].position.z = -7;
            state.AI[i].position.y = 0.5f;
            state.AI[i].rotation.z += 0.5f;

        }
        else {  // other cars are static in the background
            state.AI[i].position.z = -10;
            state.AI[i].position.y = -1.5f;
            state.AI[i].rotation.z = 140;
        }
    }
}

void VehicleSelection::Render(ShaderProgram* program) {

    for (int i = 0; i < 3; ++i) {
        state.AI[i].Render(program);
        state.AI[i].backLeftWheel->Render(program);
        state.AI[i].backRightWheel->Render(program);
        state.AI[i].frontLeftWheel->Render(program);
        state.AI[i].frontRightWheel->Render(program);
    }

}