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
#include <vector>

enum EntityType { PLAYER, ENEMY, FLOOR, BULLET, COIN, BALLOON };

class Entity {
public:
    EntityType entityType;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool isActive = true;
    int lives = 3;
    int remainingCoins = 30;

    // For bullets
    glm::vec3 trajectory;
    
    float speed;

    bool billboard;
    float width;
    float height;
    float depth;

    GLuint textureID;
    Mesh* mesh;
    
    glm::mat4 modelMatrix;
    
    Entity();
    
    bool CheckCollision(Entity* other);
    void Update(float deltaTime, Entity* player, std::vector<Entity*>& objects, std::vector<Entity*>& bullets, std::vector<Entity*>& enemies);
    void Render(ShaderProgram *program);
    void DrawBillboard(ShaderProgram* program);
};



