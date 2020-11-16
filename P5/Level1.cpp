#include "Level1.h"

#define LEVEL1_WIDTH 50
#define LEVEL1_HEIGHT 8

unsigned int level1_data[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 113, 114, 115, 145, 146, 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 132, 133, 134, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 148, 149, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 113, 114, 115, 145, 146, 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 132, 133, 134, 0, 0, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 145, 146, 0, 0, 0, 0, 113, 114, 0, 146, 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 148, 149, 150, 0, 0, 0, 150, 0, 0, 9, 10, 11, 12, 0, 0, 0, 0, 159, 160, 0, 159, 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 113, 114, 115, 145, 146, 147, 113, 114, 115, 145, 146, 147, 0, 0, 150, 0, 0, 0, 150, 0, 0, 25, 26, 27, 28, 25, 26, 25, 26, 175, 176, 0, 175, 176, 25, 26, 25, 26, 0, 0, 0, 0, 175, 176, 175, 176, 175, 176, 175, 176 };


void Level1::Initialize() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    state.music = Mix_LoadMUS(".\\Assets\\Level1Music.wav");
    Mix_PlayMusic(state.music, -1);

    // Initialize map
    GLuint mapTextureID = GameUtil::LoadTexture(".\\Assets\\tile_jungle_plants_objects.png");
    for (int i = 0; i < 400; i++) {
        if (level1_data[i] != 0) { level1_data[i] -= 1; }
    }
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 16, 13);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->startingPosition = glm::vec3(1, -6, 0);      // Track starting pos. so when char is killed, can return there
    // state.player->startingPosition = glm::vec3(47, -5, 0);   // temp
    state.player->position = state.player->startingPosition;
    state.player->speed = 2.5f;
    state.player->textureID = GameUtil::LoadTexture(".\\Assets\\Girl2.png");
    state.player->animCols = 12;
    state.player->animRows = 1;
    state.player->animRight = { 6, 7, 8 };
    state.player->animLeft = { 3, 4, 5 };
    state.player->currentAnimation = state.player->animRight;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->jumpPower = 6.5f;

    // Initialize Enemies
    Entity* ostrich = new Entity();
    ostrich->textureID = GameUtil::LoadTexture(".\\Assets\\Ostrich.png");
    ostrich->entityType = ENEMY;
    ostrich->position = glm::vec3(17.5, -2, 0);
    ostrich->animJumping = { 0, 1, 2, 3, 4, 5 };
    ostrich->currentAnimation = ostrich->animJumping;
    ostrich->animCols = 12;
    ostrich->animRows = 8;
    ostrich->speed = 420.7135114f;     // Very important. DO NOT CHANGE!!! 
    ostrich->jumpPower = 3.0f;         // I'M NOT JOKING!!!!
    ostrich->width = 0.5f;
    ostrich->enemyType = JUMPER;
    state.enemies.push_back(ostrich);


    Entity* tiger = new Entity();
    tiger->textureID = GameUtil::LoadTexture(".\\Assets\\Tiger.png");
    tiger->entityType = ENEMY;
    tiger->position = glm::vec3(10, -6, 0);
    tiger->speed = 1.0f;
    tiger->enemyType = WALKER;
    tiger->animCols = 12;
    tiger->animRows = 8;
    tiger->animLeft = { 63, 64, 65 };
    tiger->animRight = { 75, 76, 77 };
    tiger->currentAnimation = tiger->animLeft;
    state.enemies.push_back(tiger);

    Entity* monkey = new Entity();
    monkey->textureID = GameUtil::LoadTexture(".\\Assets\\Monkey.png");
    monkey->entityType = ENEMY;
    monkey->position = glm::vec3(47, -3, 0);
    monkey->speed = 1.0f;
    monkey->enemyType = SHOOTER;
    monkey->enemyState = ATTACKING;
    monkey->animCols = 12;
    monkey->animRows = 8;
    monkey->animLeft = { 63, 64, 65 };
    monkey->animRight = { 75, 76, 77 };
    monkey->currentAnimation = monkey->animLeft;
    state.enemies.push_back(monkey);

    // Initialize weapons

    Entity* banana = new Entity();
    banana->textureID = GameUtil::LoadTexture(".\\Assets\\BananaSpin.png");
    banana->entityType = WEAPON;
    banana->acceleration = glm::vec3(0);
    banana->animCols = 10;
    banana->animRows = 1;
    banana->animLeft = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    banana->animRight = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    banana->currentAnimation = banana->animLeft;
    banana->direction.x = -1.0f;
    banana->speed = 1.0f;
    banana->height = 0.5f;
    banana->width = 0.9f;
    monkey->weapon = banana;
    banana->position = monkey->position;
    banana->startingPosition = monkey->position;
    state.weapon = banana;

    // Initialize coins
    std::vector<glm::vec3> coinPositions =
    { glm::vec3(7, -5, 0), glm::vec3(7.5f, -4.5f, 0), glm::vec3(8, -4.5f, 0), glm::vec3(8.5f, -4.5f, 0), glm::vec3(9, -5, 0), glm::vec3(12.5, -4, 0), glm::vec3(13, -4, 0), glm::vec3(13.5, -4, 0), glm::vec3(21, -5, 0), glm::vec3(21.5f, -4.5f, 0),  glm::vec3(22, -4.5f, 0), glm::vec3(21, -1, 0), glm::vec3(21.5, -1, 0), glm::vec3(22, -1, 0), glm::vec3(22.5, -1, 0), glm::vec3(23, -1, 0), glm::vec3(23.5, -1, 0), glm::vec3(24, -1, 0), glm::vec3(22.5f, -5, 0), glm::vec3(30, -3, 0), glm::vec3(30.5f, -3, 0), glm::vec3(31, -3, 0), glm::vec3(31.5, -3, 0), glm::vec3(45, -6, 0), glm::vec3(45.5, -6, 0), glm::vec3(46, -6, 0), glm::vec3(46.5, -6, 0), glm::vec3(47, -6, 0), glm::vec3(48, -3, 0), glm::vec3(49, -3, 0) };
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
void Level1::Update(float deltaTime) {
    state.player->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map);
    if (state.player->position.x >= 49) { state.nextScene = 2; } // End of level
    for (Entity* enemy : state.enemies) { enemy->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map); }
    for (Entity* coin : state.coins) { coin->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map); }
    state.weapon->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map);
}
void Level1::Render(ShaderProgram* shaderProgram) {
    state.map->RenderMap(shaderProgram);
    state.player->RenderEntity(shaderProgram);
    for (Entity* enemy : state.enemies) { enemy->RenderEntity(shaderProgram); }
    for (Entity* coin : state.coins) { coin->RenderEntity(shaderProgram); }
    state.weapon->RenderEntity(shaderProgram);

    //// Temporary code to draw text on screen for debugging
    //std::stringstream xPosString, yPosString;
    //xPosString << std::fixed << std::setprecision(1) << state.player->position.x;
    //yPosString << std::fixed << std::setprecision(1) << state.player->position.y;
    //GLuint fontTextureID = GameUtil::LoadTexture(".\\Assets\\font1.png");
    //GameUtil::DrawText(shaderProgram, fontTextureID, "Player x Position: " + xPosString.str(), 0.4f, -0.20f, glm::vec3(state.player->position.x - 4, -0.5, 0));
    //GameUtil::DrawText(shaderProgram, fontTextureID, "Player y Position: " + yPosString.str(), 0.4f, -0.20f, glm::vec3(state.player->position.x - 4, -1, 0));
}