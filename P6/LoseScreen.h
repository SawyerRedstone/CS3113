#pragma once
#include "Scene.h"

class LoseScreen : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram* shaderProgram) override;
};