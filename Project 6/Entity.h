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
#include "Mesh.h"

enum EntityType { PLAYER, FLOOR, AI, WHEEL };
enum CarType {FOCUS, EVO, SEAT };

class Entity {
public:
    EntityType entityType;
    CarType carType;

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;
    glm::vec3 scale;

    float speed;
    float velocity_mph; // velocity (z) in miles per hour

    bool billboard;

    bool crashed;   // flag to let prgram know if player crashed

    float width;
    float height;
    float depth;

    GLuint textureID;

    Mesh* mesh;

    glm::mat4 modelMatrix;

    Entity* backLeftWheel;  // wheels are their own entity
    Entity* backRightWheel;
    Entity* frontLeftWheel;
    Entity* frontRightWheel;

    Entity();

    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* AI, int AIcount);
    void Render(ShaderProgram* program);

    bool CheckCollision(Entity* other);

    void buildCar(std::vector<Mesh*>& meshes, std::vector<GLuint>& textures, std::vector<Mesh*>& wheel_meshes, int mesh_index, int texture_index);
    void buildWheels(std::vector<Mesh*>& wheel_meshes);

    void DrawBillboard(ShaderProgram* program);
};