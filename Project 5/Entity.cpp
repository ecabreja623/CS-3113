#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) {
    if (other == this) return false;

    if (isActive == false || other->isActive == false) return false;

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        return true;
    }

    return false;
}

void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        collided = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        collided = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        collided = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        collided = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        collided = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        collided = true;
    }
}

void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
        collided = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
        collided = true;
    }
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];

        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {   // collided going up
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                collided = true;
            }
            else if (velocity.y < 0) {  // collided going down
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                collided = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {   // collided going right
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                collided = true;
            }
            else if (velocity.x < 0) {  // collided going left
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                collided = true;
            }
        }
    }
}

void Entity::CheckAICollision(Entity* object) { // sets collision bools for each collision
    if (CheckCollision(object))
    {
        float ydist = fabs(position.y - object->position.y);
        if (velocity.y > 0) {   // collided going up
            collidedTop = true;
            collided = true;
        }
        else if (velocity.y < 0) {  // collided going down
            collidedBottom = true;
            collided = true;
        }

        float xdist = fabs(position.x - object->position.x);
        if (velocity.x > 0) {   // collided going right
            collidedRight = true;
            collided = true;
        }
        else if (velocity.x < 0) {  // collided going left
            collidedLeft = true;
            collided = true;
        }
    }
}

void Entity::CheckProjectileCollision(Entity* objects, int objectCount, Map* map) {
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object)) {
            isActive = false;
            object->isActive = false;
        }
    }
    if (isActive) {
        CheckCollisionsX(map);
        CheckCollisionsY(map);
        if (collided) isActive = false;
    }
}

void Entity::AI(Entity* player) {
    if (CheckCollision(player)) {    // AI has touched player

        player->lives -= 1;
        player->position = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    switch (aiType) {
    case RUSHER:
        AIRusher(player);
        break;

    case WAITANDGO:
        AIWaitAndGo(player);
        break;

    case PATROLLER:
        AIPatroller();
        break;
    }
}

void Entity::AIRusher(Entity* player) { // moves towards player at all times
    if (player->position.x < position.x) {
        movement = glm::vec3(-1, 0, 0);
    }
    else {
        movement = glm::vec3(1, 0, 0);
    }
}

void Entity::AIWaitAndGo(Entity* player) {  // moves towards player after player gets within a certain distance
    switch (aiState) {
    case IDLE:
        if (glm::distance(position.x, player->position.x) < 3.0f) {
            aiState = RUNNING;
        }
        break;

    case RUNNING:
        if (player->position.x < position.x) {
            movement = glm::vec3(-1, 0, 0);
        }
        else {
            movement = glm::vec3(1, 0, 0);
        }
        break;

    case PATROLLING:
        break;
    }
}

void Entity::AIPatroller() { // move in a specific route
    if (goLeft) {
        movement = glm::vec3(-1, 0, 0);
        if (position.x < -3) {
            goLeft = false;
        }
    }

    else {
        movement = glm::vec3(1, 0, 0);
        if (position.x > 3) {
            goLeft = true;
        }
    }
}

void Entity::Projectile(Entity* objects, int objectCount, Map* map) {
    CheckProjectileCollision(objects, objectCount, map); // if projectile hits anything, remove projectile and enemy if it hits enemy
}

void Entity::Item(Entity* player) {
    if (CheckCollision(player)) {
        player->itemObtained = true;
        isActive = false;
    }
}

void Entity::Door(Entity* player) {
    if (player->itemObtained) {
        accessGranted = true;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map)
{
    if (isActive == false) return;

    collidedTop = false;    // sets all to false and checks after movement
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == ENEMY) {
        AI(player);
    }
    else if (entityType == PROJECTILE) {
        Projectile(objects, objectCount, map);
    }
    else if (entityType == ITEM) {
        Item(player);
    }
    else if (entityType == DOOR) {
        Door(player);
    }
    else if (entityType == PLAYER){  // PLAYER
        for (int i = 0; i < objectCount; ++i) {
            if (CheckCollision(&objects[i])) {  // PLAYER COLLIDES WITH AI
                player->lives -= 1;
                player->position = glm::vec3(1.0f, 0.0f, 0.0f);
            }
        }
    }

    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    if (jump) {
        jump = false;   // jump is handled
        velocity.y += jumpPower;
    }

    velocity.x = movement.x * speed;    // instant velocity/movement, velocity.y handled by gravity
    velocity += acceleration * deltaTime;   // acceleration = change in velocity over time
    position += velocity * deltaTime;   // velocity = change in position over time

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount); // Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount); // Fix if needed

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    if (isActive == false) return;

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}