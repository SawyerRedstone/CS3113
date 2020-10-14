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
#include <SDL_mixer.h>
#include <vector>

enum EntityType { PLAYER, WALL, LANDINGPAD };

class Entity {
public:
    GLuint textureID;
    glm::mat4 modelMatrix;
    float width = 1;
    float height = 1;
    EntityType type;
    glm::vec3 position;

    Entity(EntityType type);
    void Render(ShaderProgram* program);
};

class Player : public Entity {
public:
    glm::vec3 movement;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float speed;

    Player();
    Entity* CheckCollision(std::vector<Entity*>&);
    void Update(float deltaTime);
};
