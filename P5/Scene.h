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
#include "GameUtil.h"
#include "Entity.h"
#include "Map.h"
#include <SDL_mixer.h>
#include <sstream> // allows for formatting floats as strings with variable number of decimal places
#include <iomanip> // related to above. I added. Can remove later. 

struct GameState {
    Map* map;
    Entity* player;
    std::vector<Entity*> enemies;
    Entity* weapon;
    std::vector<Entity*> coins;
    Entity* lover;
    Mix_Music* music;
    int nextScene = -1; //default value for no scene change
};

class Scene {
public:
    GameState state;
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram* shaderProgram) = 0;
};

