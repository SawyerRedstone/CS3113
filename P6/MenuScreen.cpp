#include "MenuScreen.h"


void MenuScreen::Initialize() { 
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;
}

void MenuScreen::Update(float deltaTime) { }

void MenuScreen::Render(ShaderProgram* shaderProgram) { 

}