#include "Level1.h"

void Level1::Initialize() {

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0.75f, 0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 1.0f;
        
    GLuint floorTextureID = Util::LoadTexture("floor.jpg");
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

    //GLuint crateTextureID = Util::LoadTexture("crate1_diffuse.png");
    //Mesh* crateMesh = new Mesh();
    //crateMesh->LoadOBJ("cube.obj", 1);
    //for (int i = 0; i < 20; i++) {
    //    Entity* crate = new Entity();
    //    crate->textureID = crateTextureID;
    //    crate->mesh = crateMesh;
    //    crate->position = glm::vec3(rand() % 20 - 10, 0.5, rand() % 20 - 10);
    //    crate->entityType = CRATE;
    //    state.objects.push_back(crate);
    //}

    Mesh* coinMesh = new Mesh;
    GLuint coinTextureID = Util::LoadTexture("coin.png");

    GLuint UFOTextureID = Util::LoadTexture("UFO.png");
    Mesh* UFOMesh = new Mesh();
    UFOMesh->LoadOBJ("UFO.obj", 1);

    for (int i = 0; i < 30; i++) {
        Entity* enemy = new Entity();
        enemy->textureID = UFOTextureID;
        enemy->mesh = UFOMesh;
        glm::vec3 position = glm::vec3(rand() % 50 - 25, 0.5, rand() % 50 - 25);
        enemy->position = position;
        enemy->scale = glm::vec3(0.02, 0.02, 0.02);
        enemy->entityType = ENEMY;
        state.enemies.push_back(enemy);

        Entity* coin = new Entity();
        coin->textureID = coinTextureID;
        coin->mesh = coinMesh;
        coin->position = position;
        coin->billboard = true;
        coin->entityType = COIN;
        state.objects.push_back(coin);
    }
}

void Level1::Update(float deltaTime) { 
    if (state.shooting == true) {
        state.shooting = false;
        GLuint bulletTextureID = Util::LoadTexture("bullet.png");
        Entity* bullet = new Entity();
        bullet->textureID = bulletTextureID;
        bullet->position = state.player->position;
        bullet->trajectory = state.player->rotation;
        bullet->billboard = true;
        bullet->entityType = BULLET;
        state.bullets.push_back(bullet);

    }

    state.player->Update(deltaTime, state.player, state.objects, state.bullets, state.enemies);
    for (Entity* object: state.objects) {
        if(object->isActive){
            object->Update(deltaTime, state.player, state.objects, state.bullets, state.enemies);
        }    
    }

    for (Entity* enemy: state.enemies) {
        enemy->Update(deltaTime, state.player, state.objects, state.bullets, state.enemies);
    }
    for (Entity* bullet : state.bullets) {
        bullet->Update(deltaTime, state.player, state.objects, state.bullets, state.enemies);
    }
}

void Level1::Render(ShaderProgram* shaderProgram) { 
    for (Entity* object : state.objects) {
        if (object->isActive) {
            object->Render(shaderProgram);
        }
    }

    for (Entity* enemy : state.enemies) {
        enemy->Render(shaderProgram);
    }
    for (Entity* bullet : state.bullets) {
        bullet->Render(shaderProgram);
    }
}