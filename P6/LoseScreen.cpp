#include "LoseScreen.h"

void LoseScreen::Initialize() {
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;
}

void LoseScreen::Update(float deltaTime) { }

void LoseScreen::Render(ShaderProgram* shaderProgram) { }
