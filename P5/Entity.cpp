#include "Entity.h"


Entity::Entity() { }

bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    return xdist < 0 && ydist < 0;
}


// Check if the calling entity is colliding vertically with any of the entities in "objects"
// If so, correct for partial intersection. 
// Returns: The entity, if any, with which it is colliding. 
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

// Same as AdjustCollisionsY (above), but for horizontal collisions. 
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

void Entity::CheckCollisionsY(Map* map) {
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
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map* map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Entity::UpdateEntity(float deltaTime, Entity* player, std::vector<Entity*> enemies, std::vector<Entity*> coins, Map* map) {
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
            if ((size_t)animIndex >= currentAnimation.size()) { animIndex = 0; }
        }
    }
    if (jump) {
        velocity.y += jumpPower;
        jump = false;
    }
    velocity.x = direction.x * speed;
    velocity += acceleration * deltaTime;
    //if (entityType == PLAYER) {velocity.y = direction.y * speed; } // FOR DEBUGGING - ALLOWS PLAYER TO MOVE UP AND DOWN. KILL LATER
    position.y += velocity.y * deltaTime;
    // AdjustCollisionsY(enemies);
    CheckCollisionsY(map);

    position.x += velocity.x * deltaTime;
    // AdjustCollisionsX(enemies);
    CheckCollisionsX(map);

    if (entityType == ENEMY) {
        CheckCollisionsY(map);
        UpdateEnemy(player);
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    if (entityType == COIN || entityType == WEAPON) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
    }
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* shaderProgram, GLuint textureID, int index) {
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v };
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(shaderProgram->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(shaderProgram->positionAttribute);
    glVertexAttribPointer(shaderProgram->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(shaderProgram->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(shaderProgram->positionAttribute);
    glDisableVertexAttribArray(shaderProgram->texCoordAttribute);
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
    case FLOATER:
        UpdateEnemyFloater();
        break;
    }
}

void Entity::UpdateEnemyWalker(Entity* player) {
    switch (enemyState) {
    case IDLE:
        if (player->position.y >= position.y - 1 && player->position.y <= position.y + 1) {
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
        break;
    case MOVING:
        break;
    case ATTACKING:
        currentAnimation = animLeft;
        if (weapon->position.x - position.x <= -5 || weapon->position.x > position.x) {
            weapon->direction.x *= -1;
        }

    };
}

void Entity::UpdateEnemyFloater() {
    if (collidedBottom) {
        currentAnimation = animUp;
        acceleration.y = 1;
    }
    if (collidedTop) {
        currentAnimation = animDown;
        acceleration.y = -1;
    }
}

void Entity::RenderEntity(ShaderProgram* shaderProgram) {

    if (isActive == false) { return; }

    shaderProgram->SetModelMatrix(modelMatrix);

    if (currentAnimation.size() != 0) {
        DrawSpriteFromTextureAtlas(shaderProgram, textureID, currentAnimation[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(shaderProgram->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(shaderProgram->positionAttribute);
    glVertexAttribPointer(shaderProgram->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(shaderProgram->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(shaderProgram->positionAttribute);
    glDisableVertexAttribArray(shaderProgram->texCoordAttribute);
}