#include "Entity.h"
#include <SDL_mixer.h>

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    rotation = glm::vec3(0);
    scale = glm::vec3(1);

    modelMatrix = glm::mat4(1.0f);
    
    speed = 0.0f;

    billboard = false;

    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;
}

bool Entity::CheckCollision(Entity* other) {
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;

    return false;
}

void Entity::Update(float deltaTime, Entity* player, std::vector<Entity*>& objects, std::vector<Entity*>& bullets, std::vector<Entity*>& enemies) {
    glm::vec3 previousPosition = position;

    if (billboard) {
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));
    }
    if (entityType == BULLET) {
        velocity.z = cos(glm::radians(trajectory.y)) * -2.0f;
        velocity.x = sin(glm::radians(trajectory.y)) * -2.0f;
    }
    if (entityType == BALLOON) {
        if (position.y <= 0.5) {
            velocity.y = 0.2;
        }
        if (position.y >= 1.5) {
            velocity.y = -0.2;
        }
    }

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    if (entityType == PLAYER || entityType == ENEMY) {
        for (Entity* object: objects) {
            // Ignore collisions with the floor
            if (object->entityType == FLOOR) continue;

            if (entityType == ENEMY && CheckCollision(object)) {
                position = previousPosition;
                break;
            }
        }
    }
    if (entityType == ENEMY) {
        velocity.z = cos(glm::radians(rotation.y)) * -1.0f;
        velocity.x = sin(glm::radians(rotation.y)) * -1.0f;
        for (Entity* bullet : bullets) {
            if (CheckCollision(bullet)) {
                //Mix_PlayChannel(-1, Mix_LoadWAV("hitEnemy2.wav"), 0);
                Mix_PlayChannel(-1, Mix_LoadWAV("killed.wav"), 0);
                position = glm::vec3(rand() % 50 - 25, 0.5, rand() % 50 - 25);
                isActive = false;
            }
        }
        if (CheckCollision(player)) {
            //Mix_PlayChannel(-1, Mix_LoadWAV("killed.wav"), 0);
            Mix_PlayChannel(-1, Mix_LoadWAV("hitEnemy2.wav"), 0);
            player->lives -= 1;
            position = glm::vec3(rand() % 50 - 25, 0.5, rand() % 50 - 25);
        }
    }

    if (entityType == COIN) {
        if (CheckCollision(player)) {
            Mix_PlayChannel(-1, Mix_LoadWAV("coin.wav"), 0);
            isActive = false;
            player->remainingCoins -= 1;
        }
    }
    else if (entityType == ENEMY) {
        rotation.y += 30 * deltaTime;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Entity::Render(ShaderProgram *program) {
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

