#include "Entity.h"


Entity::Entity() { }

bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    return xdist < 0 && ydist < 0;
}

Entity* Entity::AdjustCollisionsY(std::vector<Entity*>& objects) {
    for (size_t i = 0; i < objects.size(); i++) {
        Entity* object = objects[i];
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                return object;
            }
            if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                return object;
            }
        }
    }
    return NULL;
}

Entity* Entity::AdjustCollisionsX(std::vector<Entity*>& objects) {
    for (size_t i = 0; i < objects.size(); i++) {
        Entity* object = objects[i];
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                return object;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                return object;
            }
        }
    }
    return NULL;
}

void Entity::Update(float deltaTime, std::vector<Entity*> platforms, Entity* player) {
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    if (isActive == false) { return; }
    if (currentAnimation.size() != 0) {
        animTime += deltaTime;
        if (animTime >= 0.25f) {
            animTime = 0.0f;
            animIndex++;
            if (animIndex >= currentAnimation.size()) { animIndex = 0; }
        }
    }

    if (jump) {
        velocity.y += jumpPower;
        jump = false;
    }

    velocity.x = direction.x * speed;
    velocity += acceleration * deltaTime;
    position.y += velocity.y * deltaTime;
    AdjustCollisionsY(platforms);
    position.x += velocity.x * deltaTime;
    AdjustCollisionsX(platforms);

    // Handle player walking off the side of the screen
    if (position.x > 5.0f) {                                                            // If player walks off right side of screen ...
        if (position.y >= -4.1f && position.y <= -2.8f) { position.y = -0.5f; }     // ... and was on bottom floor, move to middle floor ...
        else if (position.y >= -0.6f && position.y <= 0.7f) { position.y = 2.91f; }     // ... and was on middle floor, move to top floor ...
        else if (position.y >= 2.9f && position.y <= 4.2f) { position.y = -4.2f; }     // ... and was on top floor, move to bottom floor ...
        position.x = -5.0f;                                                             // ... then shift them back to the left side
    }
    if (position.x < -5.0f) position.x = -5.0f;            // Cannot walk further than left edge of any level

    if (entityType == ENEMY) {
        AdjustCollisionsY(platforms);
        UpdateEnemy(player);
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index) {
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v };
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

void Entity::UpdateEnemy(Entity* player) {
    switch (enemyType) {
    case WALKER:
        UpdateEnemyWalker(player);
        break;
    case JUMPER:
        UpdateEnemyJumper();
        break;
    case SHOOTER:
        UpdateEnemyShooter(player);
        break;
    }
}

void Entity::UpdateEnemyWalker(Entity* player) {
    switch (enemyState) {
    case IDLE:
        if (player->position.y >= -0.6f && player->position.y <= 0.7f) {
            enemyState = MOVING;
        }
        break;
    case MOVING:
        if (player->position.x < position.x) {
            currentAnimation = animLeft;
            direction = glm::vec3(-1, 0, 0);
        }
        else {
            currentAnimation = animRight;
            direction = glm::vec3(1, 0, 0);
        }
        break;
    case ATTACKING:
        break;
    }
}

void Entity::UpdateEnemyJumper() {
    if (collidedBottom) {
        jump = true;
    }
}

void Entity::UpdateEnemyShooter(Entity* player) {
    switch (enemyState) {
    case IDLE:
        if (player->position.y >= 2.9f) { enemyState = ATTACKING; }
        break;
    case MOVING:
        break;
    case ATTACKING:
        if (player->position.y < 2.9f) {           // If Player is no longer on the same floor, stop moving
            currentAnimation = animIdle;
            animIndex = 0;
            enemyState = IDLE;
        }
        else {
            currentAnimation = animLeft;
            weapon->isActive = true;
            if (weapon->position.x <= -5) {
                weapon->position.x = position.x;
            }
        }
        break;
    }
}

void Entity::Render(ShaderProgram* program) {

    if (isActive == false) { return; }

    program->SetModelMatrix(modelMatrix);

    if (currentAnimation.size() != 0) {
        DrawSpriteFromTextureAtlas(program, textureID, currentAnimation[animIndex]);
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