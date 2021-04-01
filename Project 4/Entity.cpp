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
        collided = true;
        return true;
    }

    return false;
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

void Entity::AI(Entity* player) {
    if (player->CheckCollision(this)) {    // player has touched AI
        player->CheckAICollision(this);

        if (player->collidedRight || player->collidedLeft || player->collidedTop){  // if player runs into AI
            defeatedPlayer = true;  // YOU LOSE
        }
        else { // if player lands on top of enemy
            isActive = false;      // ENEMY KILLED
        }

        if (isActive) {
            CheckAICollision(player);
            if (collidedTop) {  // if player lands on top of AI
                isActive = false;  // ENEMY KILLED
            }
            else { // if AI gets touched anywhere else, player loses
                defeatedPlayer = true;          // YOU LOSE
            }
        }
    }

    switch (aiType) {
    case RUSHER:
        AIRusher(player);
        break;

    case WAITANDGO:
        AIWaitAndGo(player);
        break;

    case PATROL:
        AIPatroler();
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

    case PATROLING:
        break;
    }
}

void Entity::AIPatroler() { // move in a specific route
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

void Entity::Update(float deltaTime, Entity* player, Entity* platforms, int platformCount)
{
    if (isActive == false) return;

    collidedTop = false;    // sets all to false and checks after movement
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == ENEMY) {
        AI(player);
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
    CheckCollisionsY(platforms, platformCount);// Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed

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

