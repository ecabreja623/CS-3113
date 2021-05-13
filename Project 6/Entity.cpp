#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    rotation = glm::vec3(0);
    scale = glm::vec3(1);

    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;

    billboard = false;
    crashed = false;

    backLeftWheel = NULL;
    backRightWheel = NULL;
    frontLeftWheel = NULL;
    frontRightWheel = NULL;

    modelMatrix = glm::mat4(1.0f);

    speed = 0.0f;
}

bool Entity::CheckCollision(Entity* other)
{
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;
    return false;
}

void Entity::buildCar(std::vector<Mesh*>& meshes, std::vector<GLuint>& textures, std::vector<Mesh*>& wheel_meshes, int mesh_index, int texture_index) {
    if (mesh_index == 0) {
        carType = FOCUS;
        mesh = meshes[0];
        textureID = textures[texture_index];
        scale = glm::vec3(0.5f, 0.5f, 0.5f);    // FOCUS has a different size to the other two
        width = 1.5;
        depth = 4.5;
        speed = 20.0f;
    }
    else if (mesh_index == 1) {
        carType = EVO;
        mesh = meshes[1];
        textureID = textures[texture_index];
        scale = glm::vec3(0.01f, 0.01f, 0.01f);
        width = 1.25;
        depth = 4;
        speed = 22.0f;  // each car has a different speed
    }
    else {
        carType = SEAT;
        mesh = meshes[2];
        textureID = textures[texture_index];
        scale = glm::vec3(0.01f, 0.01f, 0.01f);
        width = 1.25;
        depth = 3.5;
        speed = 18.0f;
    }

    buildWheels(wheel_meshes);  // initialize wheels
}

void Entity::buildWheels(std::vector<Mesh*>& wheel_meshes) {
    backLeftWheel = new Entity();
    backLeftWheel->entityType = WHEEL;
    backLeftWheel->textureID = textureID;   // textures have texture of wheels as well
    backLeftWheel->scale = glm::vec3(0.01f, 0.01f, 0.01f);

    backRightWheel = new Entity();
    backRightWheel->entityType = WHEEL;
    backRightWheel->textureID = textureID;
    backRightWheel->scale = glm::vec3(0.01f, 0.01f, 0.01f);

    frontLeftWheel = new Entity();
    frontLeftWheel->entityType = WHEEL;
    frontLeftWheel->textureID = textureID;
    frontLeftWheel->scale = glm::vec3(0.01f, 0.01f, 0.01f);

    frontRightWheel = new Entity();
    frontRightWheel->entityType = WHEEL;
    frontRightWheel->textureID = textureID;
    frontRightWheel->scale = glm::vec3(0.01f, 0.01f, 0.01f);


    if (carType == FOCUS) {
        backLeftWheel->mesh = wheel_meshes[0];
        backRightWheel->mesh = wheel_meshes[0];
        frontLeftWheel->mesh = wheel_meshes[0];
        frontRightWheel->mesh = wheel_meshes[0];
    }

    else {  // EVO and SEAT use same wheel mesh
        backLeftWheel->mesh = wheel_meshes[1];
        backRightWheel->mesh = wheel_meshes[1];
        frontLeftWheel->mesh = wheel_meshes[1];
        frontRightWheel->mesh = wheel_meshes[1];
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* ai, int AIcount)
{
    glm::vec3 previousPosition = position;  // stores current position

    if (billboard) {    // if billboard, rotate it so that it is always facing the player
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));
    }

    velocity += acceleration * deltaTime;
    position += velocity * speed * deltaTime;

    velocity_mph = -(abs((position.z - previousPosition.z)) / 5280) / (deltaTime/3600);   // distance over time, miles / hour

    if (entityType == PLAYER || entityType == AI) {
        if (carType == FOCUS) {

            // Updates position of wheels so they stay on same spot as car moves and rotates
            // x′ = (x−p)cos(0) − (y−q)sin(0) + p   (x', y' = updated position; p, q = center of car)
            // y′ = (x−p)sin(0) + (y−q)cos(0) + q
            // FOCUS
            backLeftWheel->position = glm::vec3(-((position.x + 0.75) - position.x) * cos(glm::radians(rotation.z)) - ((position.z + 1.1375) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x + 0.75) - position.x) * sin(glm::radians(rotation.z)) + ((position.z + 1.1375) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            backRightWheel->position = glm::vec3(-((position.x - 0.75) - position.x) * cos(glm::radians(rotation.z)) - ((position.z + 1.1375) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x - 0.75) - position.x) * sin(glm::radians(rotation.z)) + ((position.z + 1.1375) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            frontLeftWheel->position = glm::vec3(-((position.x + 0.75) - position.x) * cos(glm::radians(rotation.z)) - ((position.z - 1.45) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x + 0.75) - position.x) * sin(glm::radians(rotation.z)) + ((position.z - 1.45) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            frontRightWheel->position = glm::vec3(-((position.x - 0.75) - position.x) * cos(glm::radians(rotation.z)) - ((position.z - 1.45) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x - 0.75) - position.x) * sin(glm::radians(rotation.z)) + ((position.z - 1.45) - position.z) * cos(glm::radians(rotation.z)) + position.z);
        }

        else if (carType == EVO) {
            //EVO
            backLeftWheel->position = glm::vec3(-((position.x + 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z + 1.0) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x + 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z + 1.0) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            backRightWheel->position = glm::vec3(-((position.x - 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z + 1.0) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x - 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z + 1.0) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            frontLeftWheel->position = glm::vec3(-((position.x + 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z - 1.3) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x + 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z - 1.3) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            frontRightWheel->position = glm::vec3(-((position.x - 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z - 1.3) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                position.y,
                ((position.x - 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z - 1.3) - position.z) * cos(glm::radians(rotation.z)) + position.z);

        }

        else if (carType == SEAT){
            //SEAT
            backLeftWheel->position = glm::vec3(-((position.x + 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z + 1.25) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                 position.y,
                 ((position.x + 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z + 1.25) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            backRightWheel->position = glm::vec3(-((position.x - 0.625)-position.x  ) * cos(glm::radians(rotation.z)) - ((position.z + 1.25) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                 position.y,
                 ((position.x - 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z + 1.25) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            frontLeftWheel->position = glm::vec3(-((position.x + 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z - 0.85) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                 position.y,
                 ((position.x + 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z - 0.85) - position.z) * cos(glm::radians(rotation.z)) + position.z);

            frontRightWheel->position = glm::vec3(-((position.x - 0.625) - position.x) * cos(glm::radians(rotation.z)) - ((position.z - 0.85) - position.z) * -sin(glm::radians(rotation.z)) + position.x,
                 position.y,
                 ((position.x - 0.625) - position.x) * sin(glm::radians(rotation.z)) + ((position.z - 0.85) - position.z) * cos(glm::radians(rotation.z)) + position.z);
        }

        backLeftWheel->rotation.y = rotation.z; // wheels always face the direction the car is looking
        backRightWheel->rotation.y = rotation.z;
        frontLeftWheel->rotation.y = rotation.z;
        frontRightWheel->rotation.y = rotation.z;
    }

    if (entityType == PLAYER) { 
        
        for (int i = 0; i < objectCount; i++)
        {
            if (CheckCollision(&objects[i])) {  //  if there is a collision, player crashed
                position = previousPosition;
                crashed = true;
                break;
            }
        }
        for (int i = 0; i < AIcount; i++)
        {
            if (CheckCollision(&ai[i])) {  //  if there is a collision, player crashed
                position = previousPosition;
                crashed = true;
                break;
            }
        }

        if (crashed) {
            float r = ((float)rand() / RAND_MAX) * (5 - -5) + -5;   // make car shake if crashed
            rotation.y = r;
            rotation.y = -r;
        }
    }

    else if (entityType == AI) {
        
        if (CheckCollision(player)) {
            position = previousPosition;    
            player->crashed;    // if AI crashes with player, player crashed
        }

        for (int i = 0; i < AIcount; i++)
        {   
            if (&ai[i] == this) continue;   // don't check collision with itself
            if (CheckCollision(&ai[i])) {  // don't allow AI cars to overlap
                position = previousPosition;
                break;
            }
        }

        if (velocity.z > 0) {   // tires rotate while moving
            backLeftWheel->rotation.x += 5;
            backRightWheel->rotation.x += 5;
            frontLeftWheel->rotation.x += 5;
            frontRightWheel->rotation.x += 5;
        }
        else if (velocity.z < 0) {
            backLeftWheel->rotation.x -= 5;
            backRightWheel->rotation.x -= 5;
            frontLeftWheel->rotation.x -= 5;
            frontRightWheel->rotation.x -= 5;
        }

    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::scale(modelMatrix, scale);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Entity::Render(ShaderProgram* program) {
    program->SetModelMatrix(modelMatrix);

    glBindTexture(GL_TEXTURE_2D, textureID);

    if (billboard) {
        DrawBillboard(program);
    }
    else {
        mesh->Render(program);
    }
}

void Entity::DrawBillboard(ShaderProgram* program) {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}