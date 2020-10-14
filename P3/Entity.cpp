#include "Entity.h"
#include <iostream>

Entity::Entity(EntityType t) : type(t) {
    position = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}

Player::Player() : Entity(PLAYER) {
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
    speed = 0;
}

Entity* Player::CheckCollision(std::vector<Entity*>& objects) {
    for (Entity* other : objects) {
        float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
        float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
        if (xdist < 0 && ydist < 0) {
            return other;                   // Return the entity that the player hit.
        }
    }
    return NULL;
}


void Player::Update(float deltaTime) {
    // Update player's location and movement
    acceleration.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::Render(ShaderProgram* program) {
    program->SetModelMatrix(modelMatrix);
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
