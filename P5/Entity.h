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
#include "Map.h"
#include <vector>

enum EntityType { PLAYER, ENEMY, COIN, WEAPON };
enum EnemyType { WALKER, JUMPER, SHOOTER, FLOATER };
enum EnemyState { IDLE, MOVING, ATTACKING };

class Entity {
public:
    EntityType entityType;
    EnemyType enemyType;
    EnemyState enemyState = IDLE;

    // Default values for Entity variables
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 direction = glm::vec3(0);
    glm::vec3 startingPosition = glm::vec3(0);
    glm::vec3 position = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0, -9.81f, 0);
    float speed = 1.0f;
    float width = 1;
    float height = 1;
    bool jump = false;
    float jumpPower = 0;
    bool isActive = true;

    GLuint textureID;
    std::vector<int> animRight;
    std::vector<int> animLeft;
    std::vector<int> animJumping;
    std::vector<int> animUp;
    std::vector<int> animDown;
    std::vector<int> animIdle;
    std::vector<int> currentAnimation;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;

    Entity* weapon;

    Entity();
    bool CheckCollision(Entity* other);
    Entity* AdjustCollisionsX(std::vector<Entity*>& objects);
    Entity* AdjustCollisionsY(std::vector<Entity*>& objects);
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);
    void UpdateEntity(float deltaTime, Entity* player, std::vector<Entity*> enemies, std::vector<Entity*> coins, Map* map);
    void RenderEntity(ShaderProgram* shaderProgram);
    void DrawSpriteFromTextureAtlas(ShaderProgram* shaderProgram, GLuint textureID, int index);

    void UpdateEnemy(Entity* player);
    void UpdateEnemyWalker(Entity* player);
    void UpdateEnemyJumper();
    void UpdateEnemyShooter(Entity* player);
    void UpdateEnemyFloater();
};