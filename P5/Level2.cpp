#include "Level2.h"

#define LEVEL2_WIDTH 50
#define LEVEL2_HEIGHT 8

unsigned int level2_data[] = {
    274, 275, 276, 277, 278, 279, 6, 6, 7, 6, 7, 6, 7, 7, 6, 7, 6, 7, 43, 401, 402, 403, 403, 6, 7, 404, 32, 33, 32, 33, 32, 32, 33, 210, 264, 6, 7, 6, 7, 403, 6, 7, 218, 401, 402, 403, 403, 7, 7, 7, 300, 301, 302, 216, 217, 218, 32, 32, 33, 32, 33, 32, 33, 33, 32, 33, 32, 33, 0, 0, 0, 0, 0, 32, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 33, 32, 33, 0, 32, 33, 0, 0, 0, 0, 0, 33, 33, 33, 326, 327, 328, 242, 243, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 210, 211, 212, 213, 193, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 352, 353, 354, 243, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 183, 184, 305, 0, 0, 0, 0, 0, 0, 0, 378, 379, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 183, 184, 185, 241, 242, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 404, 405, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 210, 211, 267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 183, 184, 305, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 183, 184, 193, 194, 195, 9, 10, 11, 12, 13, 237, 293, 0, 0, 0, 0, 0, 0, 0, 56, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 122, 123, 317, 193, 194, 195, 35, 36, 36, 35, 36, 37, 38, 39, 35, 36, 37, 38, 39, 263, 319, 34, 35, 34, 35, 36, 37, 38, 39, 183, 184, 185, 193, 194, 195, 34, 35, 38, 35, 36, 37, 38, 34, 183, 184, 305, 195, 38, 35, 38 };

void Level2::Initialize() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    //state.music = Mix_LoadMUS(".\\Assets\\UnderwaterLevelMusic.wav");
    state.music = Mix_LoadMUS(".\\Assets\\Level1Music.wav");
    Mix_PlayMusic(state.music, -1);

    // Initialize map
    GLuint mapTextureID = GameUtil::LoadTexture(".\\Assets\\CavePlatformer.png");
    for (int i = 0; i < 400; i++) {
        if (level2_data[i] != 0) { level2_data[i] -= 1; }
    }
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 26, 16);
    glClearColor(0.0f, 0.2f, 1.0f, 1.0f);
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->startingPosition = glm::vec3(1, -6, 0);      // Track starting pos. so when char is killed, can return there
    state.player->position = state.player->startingPosition;
    state.player->acceleration = glm::vec3(0, -2.0f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = GameUtil::LoadTexture(".\\Assets\\DiverLR.png");
    state.player->animCols = 7;
    state.player->animRows = 2;
    state.player->animLeft = { 0, 1, 2, 3, 4, 5, 6 };
    state.player->animRight = { 7, 8, 9, 10, 11, 12, 13 };
    state.player->currentAnimation = state.player->animRight;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->height = 0.3f;
    state.player->width = 0.6f;
    state.player->jumpPower = 2.0f;

    // Initialize Enemies
    Entity* shark = new Entity();
    shark->entityType = ENEMY;
    shark->enemyType = FLOATER;
    shark->textureID = GameUtil::LoadTexture(".\\Assets\\Shark.png");
    shark->animCols = 3;
    shark->animRows = 4;
    shark->position = glm::vec3(48, -6, 0);
    shark->animUp = { 9, 10, 11 };
    shark->animDown = { 0, 1, 2 };
    shark->currentAnimation = shark->animUp;
    shark->acceleration = glm::vec3(0, 1.0f, 0);
    state.enemies.push_back(shark);

    Entity* stripeFish = new Entity();
    stripeFish->entityType = ENEMY;
    stripeFish->enemyType = FLOATER;
    stripeFish->textureID = GameUtil::LoadTexture(".\\Assets\\BigFish.png");
    stripeFish->animCols = 12;
    stripeFish->animRows = 8;
    stripeFish->position = glm::vec3(15, -5, 0);
    stripeFish->animUp = { 45, 46, 47 };
    stripeFish->animDown = { 9, 10, 11 };
    stripeFish->currentAnimation = stripeFish->animUp;
    stripeFish->acceleration = glm::vec3(0, 1.0f, 0);
    state.enemies.push_back(stripeFish);

    Entity* redFish = new Entity();
    redFish->entityType = ENEMY;
    redFish->enemyType = FLOATER;
    redFish->textureID = GameUtil::LoadTexture(".\\Assets\\LittleFish.png");
    redFish->animCols = 12;
    redFish->animRows = 8;
    redFish->position = glm::vec3(24, -6, 0);
    redFish->animUp = { 90, 91, 92 };
    redFish->animDown = { 54, 55, 56 };
    redFish->currentAnimation = redFish->animUp;
    redFish->acceleration = glm::vec3(0, 1.0f, 0);
    state.enemies.push_back(redFish);

    Entity* blueFish = new Entity();
    blueFish->entityType = ENEMY;
    blueFish->enemyType = FLOATER;
    blueFish->textureID = GameUtil::LoadTexture(".\\Assets\\LittleFish.png");
    blueFish->animCols = 12;
    blueFish->animRows = 8;
    blueFish->position = glm::vec3(24, -5, 0);
    blueFish->animUp = { 36, 37, 38 };
    blueFish->animDown = { 0, 1, 2 };
    blueFish->currentAnimation = blueFish->animUp;
    blueFish->acceleration = glm::vec3(0, 1.0f, 0);
    state.enemies.push_back(blueFish);

    // Initialize coins
    //coin->position = glm::vec3(8, -4.5, 0);
    std::vector<glm::vec3> coinPositions = { glm::vec3(3, -5, 0), glm::vec3(3.5f, -5, 0), glm::vec3(4, -5, 0), glm::vec3(5, -4, 0), glm::vec3(5.5, -4, 0), glm::vec3(7, -3, 0),
        glm::vec3(7.5, -3, 0), glm::vec3(8, -3, 0), glm::vec3(8.5, -3, 0), glm::vec3(10, -4, 0), glm::vec3(10.5, -4, 0), glm::vec3(14.5, -2.5, 0), glm::vec3(15, -2, 0),
        glm::vec3(15.5, -2.5, 0), glm::vec3(19, -2, 0), glm::vec3(19.5, -2, 0), glm::vec3(20, -2, 0), glm::vec3(23.5, -3, 0), glm::vec3(24, -2.5, 0), glm::vec3(24.5, -3, 0),
        glm::vec3(31, -5, 0), glm::vec3(31.5, -5, 0), glm::vec3(32, -5, 0), glm::vec3(32.5, -5, 0), glm::vec3(40, -5, 0), glm::vec3(40.5, -5, 0), glm::vec3(41, -5, 0),
        glm::vec3(41.5, -5, 0), glm::vec3(44, -4, 0), glm::vec3(44.5, -4, 0), glm::vec3(45, -4, 0), glm::vec3(45.5, -4, 0), };
    for (glm::vec3 coinPosition : coinPositions) {
        Entity* coin = new Entity();
        coin->textureID = GameUtil::LoadTexture(".\\assets\\CoinSpin.png");  //coin->textureID = GameUtil::LoadTexture(".\\assets\\CTG Coin 2.png");
        coin->entityType = COIN;
        coin->acceleration = glm::vec3(0);
        coin->animCols = 4;
        coin->animRows = 3;
        coin->animIdle = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
        coin->currentAnimation = coin->animIdle;
        coin->position = coinPosition;
        state.coins.push_back(coin);
    }

}

void Level2::Update(float deltaTime) {
    state.player->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map);
    if (state.player->position.x >= 49) { state.nextScene = 3; } // End of level
    for (Entity* enemy : state.enemies) { enemy->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map); }
    for (Entity* coin : state.coins) { coin->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map); }
}
void Level2::Render(ShaderProgram* shaderProgram) {
    state.map->RenderMap(shaderProgram);
    state.player->RenderEntity(shaderProgram);
    for (Entity* enemy : state.enemies) { enemy->RenderEntity(shaderProgram); }
    for (Entity* coin : state.coins) { coin->RenderEntity(shaderProgram); }
    //// Draw text
    //std::stringstream xPosString, yPosString;
    //xPosString << std::fixed << std::setprecision(1) << state.player->position.x;
    //yPosString << std::fixed << std::setprecision(1) << state.player->position.y;
    //GLuint fontTextureID = GameUtil::LoadTexture(".\\Assets\\font1.png");
    //GameUtil::DrawText(shaderProgram, fontTextureID, "Player x Position: " + xPosString.str(), 0.4f, -0.20f, glm::vec3(state.player->position.x - 4, -0.5, 0));
    //GameUtil::DrawText(shaderProgram, fontTextureID, "Player y Position: " + yPosString.str(), 0.4f, -0.20f, glm::vec3(state.player->position.x - 4, -1, 0));
}