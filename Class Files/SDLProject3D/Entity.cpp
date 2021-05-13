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

//void Entity::Update(float deltaTime)
void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount)
{
    glm::vec3 previousPosition = position;  // stores current position

    if (billboard) {    // if billboard, rotate it so that it is always facing the player
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));

        velocity.z = cos(glm::radians(rotation.y)) * -1.0f; // billboards will walk towards player
        velocity.x = sin(glm::radians(rotation.y)) * -1.0f;
    }

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    if (entityType == PLAYER) { // only care if player collides with things
        for (int i = 0; i < objectCount; i++)
        {
            // Ignore collisions with the floor
            if (objects[i].entityType == FLOOR) continue;
            if (CheckCollision(&objects[i])) {  //  if there is a collision go back to previous position
                position = previousPosition;
                break;
            }
        }
    }
    else if (entityType == CUBE) {
        rotation.x += 45 * deltaTime;   // keep increasing rotation.x to make it continue rotating , negative value rotates in opposite direction
        
        rotation.y += 45 * deltaTime;
    }
    else if (entityType == ENEMY) {
        rotation.y += 30 * deltaTime;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::scale(modelMatrix, scale);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Entity::Render(ShaderProgram* program) {
    program->SetModelMatrix(modelMatrix);

    //float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };      not needed anymore
    //float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };               in entity.h

    glBindTexture(GL_TEXTURE_2D, textureID);

    /* just need mesh->render
    //glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glVertexAttribPointer(program->positionAttribute, 3, GL_FLOAT, false, 0, vertices); // xyz now instead of xy
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);    // still only uv
    glEnableVertexAttribArray(program->texCoordAttribute);

    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    */  

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