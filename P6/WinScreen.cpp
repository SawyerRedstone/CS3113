#include "WinScreen.h"

void WinScreen::Initialize() {
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;

    GLuint floorTextureID = Util::LoadTexture("moon2.jpg");
    Mesh* cubeMesh = new Mesh;
    cubeMesh->LoadOBJ("cube.obj", 50);

    Entity* floor = new Entity();
    floor->textureID = floorTextureID;
    floor->mesh = cubeMesh;
    floor->position = glm::vec3(0, -0.25, 0);
    floor->rotation = glm::vec3(0, 0, 0);
    floor->acceleration = glm::vec3(0, 0, 0);
    floor->scale = glm::vec3(50, 0.5f, 50);
    floor->entityType = FLOOR;
    state.objects.push_back(floor);

    GLuint blue1TextureID = Util::LoadTexture("blueBalloon.png");
    GLuint blue2TextureID = Util::LoadTexture("blueBalloon2.png");
    GLuint redTextureID = Util::LoadTexture("redBalloon.png");
    GLuint greenTextureID = Util::LoadTexture("greenBalloon.png");
    GLuint balloonsTextureID = Util::LoadTexture("balloons.png");

    for (int i = 0; i < 20; i++) {
        Entity* blue = new Entity();
        blue->textureID = blue1TextureID;
        blue->position = glm::vec3(rand() % 50 - 25, 0.5 + rand() % 2, rand() % 50 - 25);;
        blue->billboard = true;
        blue->entityType = BALLOON;
        state.objects.push_back(blue);

        Entity* blue2 = new Entity();
        blue2->textureID = blue2TextureID;
        blue2->position = glm::vec3(rand() % 50 - 25, 0.5 + rand() % 2, rand() % 50 - 25);;
        blue2->billboard = true;
        blue2->entityType = BALLOON;
        state.objects.push_back(blue2);

        Entity* red = new Entity();
        red->textureID = redTextureID;
        red->position = glm::vec3(rand() % 50 - 25, 0.5 + rand() % 2, rand() % 50 - 25);;
        red->billboard = true;
        red->entityType = BALLOON;
        state.objects.push_back(red);

        Entity* green = new Entity();
        green->textureID = greenTextureID;
        green->position = glm::vec3(rand() % 50 - 25, 0.5 + rand() % 2, rand() % 50 - 25);;
        green->billboard = true;
        green->entityType = BALLOON;
        state.objects.push_back(green);

        Entity* balloons = new Entity();
        balloons->textureID = balloonsTextureID;
        balloons->position = glm::vec3(rand() % 50 - 25, 0.5 + rand() % 2, rand() % 50 - 25);;
        balloons->billboard = true;
        balloons->entityType = BALLOON;
        state.objects.push_back(balloons);
    }
}

void WinScreen::Update(float deltaTime) { 
    state.player->Update(deltaTime, state.player, state.objects, state.bullets, state.enemies);
    for (Entity* object : state.objects) {
        object->Update(deltaTime, state.player, state.objects, state.bullets, state.enemies);
    }
}

void WinScreen::Render(ShaderProgram* shaderProgram) { 
    for (Entity* object : state.objects) {
        object->Render(shaderProgram);
    }
}