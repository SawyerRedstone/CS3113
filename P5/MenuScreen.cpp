#include "MenuScreen.h"

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

unsigned int menu_data[] = {
    77, 78, 79, 0, 0, 0, 0, 241, 242, 242, 242, 242, 243, 0, 105, 106, 107, 77, 78, 79, 0, 214, 326, 242, 242, 327, 0, 0, 133, 134, 135, 105, 106, 107, 241, 242, 243, 190, 191, 333, 334, 0, 0, 0, 0, 133, 134, 135, 0, 272, 217, 218, 219, 222, 0, 0, 299, 0, 0, 0, 0, 0, 0, 0, 0, 246, 247, 0, 0, 0, 242, 299, 0, 0, 0, 0, 0, 0, 0, 274, 275, 0, 0, 0, 242, 327, 0, 0, 0, 0, 0, 0, 0, 330, 331, 0, 0, 0, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31 };

void MenuScreen::Initialize() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    state.music = Mix_LoadMUS(".\\Assets\\Level1Music.wav");
    Mix_PlayMusic(state.music, -1);

    // Initialize map
    GLuint mapTextureID = GameUtil::LoadTexture(".\\Assets\\Jungle_terrain.png");
    for (int i = 0; i < 112; i++) {
        if (menu_data[i] != 0) { menu_data[i] -= 1; }
    }
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 28, 15);

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->startingPosition = glm::vec3(3, -5, 0);      // Track starting pos. so when char is killed, can return there
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
}

void MenuScreen::Update(float deltaTime) {
    state.player->UpdateEntity(deltaTime, state.player, state.enemies, state.coins, state.map);
}

void MenuScreen::Render(ShaderProgram* shaderProgram) {
    state.map->RenderMap(shaderProgram);
    state.player->RenderEntity(shaderProgram);
    GLuint fontTextureID = GameUtil::LoadTexture(".\\Assets\\font1.png");
    GameUtil::DrawText(shaderProgram, fontTextureID, "Time for your wedding! Can you make it in time?", 0.4f, -0.2f, glm::vec3(0.2, -3.5, 0));
    GameUtil::DrawText(shaderProgram, fontTextureID, "Press Enter to start", 0.6f, -0.25f, glm::vec3(1.2, -4, 0));
}