#pragma once
#include "Scene.h"

class MenuScreen : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram* shaderProgram) override;
};