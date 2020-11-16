#include "Level3.h"

#define LEVEL3_WIDTH 50
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] = {
    745, 0, 747, 748, 749, 750, 0, 752, 745, 746, 0, 748, 749, 750, 0, 0, 747, 745, 746, 0, 0, 0, 0, 745, 746, 747, 748, 0, 0, 0, 747, 748, 749, 750, 0, 0, 747, 748, 749, 750, 745, 746, 0, 0, 0, 747, 748, 749, 750, 751, 757, 0, 759, 760, 761, 762, 763, 764, 757, 758, 759, 760, 761, 0, 0, 0, 759, 757, 758, 0, 0, 0, 0, 757, 758, 759, 760, 761, 757, 0, 759, 760, 761, 762, 0, 0, 759, 760, 761, 762, 757, 758, 759, 757, 0, 759, 760, 761, 762, 763, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 247, 248, 249, 250, 0, 0, 0, 0, 0, 0, 0, 0, 0, 558, 559, 560, 561, 560, 563, 0, 0, 0, 0, 0, 0, 0, 721, 721, 0, 0, 0, 0, 0, 723, 723, 723, 723, 0, 0, 0, 0, 0, 0, 0, 133, 134, 135, 136, 0, 0, 572, 0, 0, 572, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 567, 0, 0, 0, 721, 721, 721, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 627, 627, 0, 0, 0, 0, 0, 0, 0, 0, 567, 0, 0, 567, 0, 0, 584, 0, 0, 584, 0, 0, 7, 8, 8, 9, 10, 0, 0, 0, 0, 0, 0, 579, 0, 0, 0, 652, 653, 654, 0, 0, 0, 0, 0, 721, 721, 0, 0, 0, 639, 639, 0, 0, 781, 782, 783, 784, 0, 0, 579, 0, 0, 579, 0, 0, 596, 0, 0, 596, 0, 0, 0, 0, 0, 572, 0, 782, 783, 782, 783, 784, 0, 591, 0, 0, 0, 652, 653, 654, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 651, 651, 0, 0 };


void Level3::Initialize() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    state.music = Mix_LoadMUS(".\\Assets\\FinalLevelMusic.wav");
    Mix_PlayMusic(state.music, -1);

    // Initialize map
    GLuint mapTextureID = GameUtil::LoadTexture(".\\Assets\\alien-planet-tileset.png");
    for (int i = 0; i < 400; i++) {
        if (level3_data[i] != 0) { level3_data[i] -= 1; }
    }
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 12, 72);
    glClearColor(0.7f, 0.5f, 0.5f, 1.0f);

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

    Entity* cactus1 = new Entity();
    cactus1->textureID = GameUtil::LoadTexture(".\\Assets\\Headless dancing cactus spritesheet.png");
    cactus1->entityType = ENEMY;
    cactus1->position = glm::vec3(27, -6, 0);
    cactus1->speed = 1.0f;
    cactus1->enemyType = WALKER;
    cactus1->animCols = 8;
    cactus1->animRows = 8;
    cactus1->animLeft = { 0, 1, 2, 3, 4, 5, 6, 7 };
    cactus1->animRight = { 0, 1, 2, 3, 4, 5, 6, 7 };
    cactus1->currentAnimation = cactus1->animLeft;
    state.enemies.push_back(cactus1);

    Entity* cactus2 = new Entity();
    cactus2->textureID = GameUtil::LoadTexture(".\\Assets\\Headless dancing cactus spritesheet.png");
    cactus2->entityType = ENEMY;
    cactus2->position = glm::vec3(47, -3, 0);
    cactus2->animJumping = { 57, 58, 59, 60, 61, 62, 63, 64 };
    cactus2->currentAnimation = cactus2->animJumping;
    cactus2->animCols = 8;
    cactus2->animRows = 8;
    cactus2->speed = 420.7135114f;     // Very important. DO NOT CHANGE!!! 
    cactus2->jumpPower = 3.0f;         // I'M NOT JOKING!!!!
    cactus2->width = 0.5f;
    cactus2->enemyType = JUMPER;
    state.enemies.push_back(cactus2);

    // Initialize coins
    std::vector<glm::vec3> coinPositions = { glm::vec3(6.5, -3, 0), glm::vec3(7, -2.5, 0), glm::vec3(7.5, -2.5, 0), glm::vec3(8, -2.5, 0),
        glm::vec3(8.5, -3, 0), glm::vec3(13, -3, 0), glm::vec3(13.5, -3, 0), glm::vec3(14, -3, 0), glm::vec3(19, -4, 0), glm::vec3(19.5, -3.5, 0), glm::vec3(20, -3.5, 0), glm::vec3(20.5, -3.5, 0), glm::vec3(21, -4, 0), glm::vec3(27, -3, 0), glm::vec3(27.5, -3, 0), glm::vec3(28, -3, 0), glm::vec3(38, -3, 0), glm::vec3(38.5, -3, 0), glm::vec3(39, -3, 0), glm::vec3(41, -5, 0), glm::vec3(41.5, -5, 0), glm::vec3(42, -5, 0) };
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
void Level3::Update(float deltaTime) {
    state.player->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map);
    if (state.player->position.x >= 49) { state.nextScene = 4; } // Right end of level
    for (Entity* enemy : state.enemies) { enemy->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map); }
    for (Entity* coin : state.coins) { coin->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map); }
}
void Level3::Render(ShaderProgram* shaderProgram) {
    state.map->RenderMap(shaderProgram);
    state.player->RenderEntity(shaderProgram);
    for (Entity* enemy : state.enemies) { enemy->RenderEntity(shaderProgram); }
    for (Entity* coin : state.coins) { coin->RenderEntity(shaderProgram); }

    //// Temporary code to draw text on screen for debugging
    //std::stringstream xPosString, yPosString;
    //xPosString << std::fixed << std::setprecision(1) << state.player->position.x;
    //yPosString << std::fixed << std::setprecision(1) << state.player->position.y;
    //GLuint fontTextureID = GameUtil::LoadTexture(".\\Assets\\font1.png");
    //GameUtil::DrawText(shaderProgram, fontTextureID, "Player x Position: " + xPosString.str(), 0.4f, -0.20f, glm::vec3(state.player->position.x - 4, -0.5, 0));
    //GameUtil::DrawText(shaderProgram, fontTextureID, "Player y Position: " + yPosString.str(), 0.4f, -0.20f, glm::vec3(state.player->position.x - 4, -1, 0));
    ////DrawText(&shaderProgram, displayText);                                       // Display current text message
}