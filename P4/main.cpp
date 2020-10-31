// Deep Dark Desert of Despair
// Rise of the AI
// CS3113 Project #4

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>

#include "ShaderProgram.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Entity.h"
#include "stb_image.h"
#include <vector>

struct GameState {
    Entity* player;
    std::vector<Entity*> platforms;
    std::vector<Entity*> enemies;
    std::vector<Entity*> signs;
    Entity* spear;
    bool gameOver = false;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram shaderProgram;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
Mix_Music* music;
std::string displayText;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) { std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false); }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram* shaderProgram, std::string text) {
    GLuint fontTextureID = LoadTexture("font1.png");
    float size = 0.5f;
    float spacing = -0.25f;
    glm::vec3 position = glm::vec3(-4.5f, 4.5f, 0.0f);
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (size_t i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), { offset + (-0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, });
        texCoords.insert(texCoords.end(), { u, v, u, v + height, u + width, v, u + width, v + height, u + width, v, u, v + height, });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    shaderProgram->SetModelMatrix(modelMatrix);
    glUseProgram(shaderProgram->programID);
    glVertexAttribPointer(shaderProgram->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(shaderProgram->positionAttribute);
    glVertexAttribPointer(shaderProgram->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(shaderProgram->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(shaderProgram->positionAttribute);
    glDisableVertexAttribArray(shaderProgram->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("The-Chase.mp3");
    Mix_PlayMusic(music, -1);

    displayWindow = SDL_CreateWindow("Deep Dark Desert of Doom!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    shaderProgram.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    shaderProgram.SetViewMatrix(viewMatrix);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -1.0f, 1.0f);
    shaderProgram.SetProjectionMatrix(projectionMatrix);
    modelMatrix = glm::mat4(1.0f);

    glUseProgram(shaderProgram.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-3, -3, 0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.5f;
    state.player->textureID = LoadTexture("Betty.png");

    state.player->animRight = { 3, 7, 11, 15 };
    state.player->animLeft = { 1, 5, 9, 13 };
    state.player->currentAnimation = state.player->animRight;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.6f;   // Guido has 0.8f;
    state.player->jumpPower = 7.0f;

    // Initialize platforms
    for (float yval = -5; yval <= 4.25; yval += 3.50) {
        for (float xval = -4.5; xval <= 4.5; xval += 1) {
            Entity* platform = new Entity();
            platform->entityType = PLATFORM;
            platform->position = glm::vec3(xval, yval, 0.0f);
            platform->textureID = LoadTexture("SquareGrass.png");
            platform->modelMatrix = glm::translate(modelMatrix, platform->position);
            state.platforms.push_back(platform);
        }
    }

    // Initialize signs
    for (float yval = -4.0f; yval <= 3.0f; yval += 3.5) {
        Entity* sign = new Entity();
        sign->position = glm::vec3(4.5f, yval, 0.0f);
        sign->textureID = LoadTexture("SignRight.png");
        sign->modelMatrix = glm::translate(modelMatrix, sign->position);
        state.signs.push_back(sign);
    }

    // Initialize Enemies
    Entity* archer = new Entity();
    archer->textureID = LoadTexture("orc.png");
    archer->entityType = ENEMY;
    archer->position = glm::vec3(3, 3, 0);
    archer->speed = 1.0f;
    archer->enemyType = SHOOTER;
    archer->animLeft = { 225, 226, 227, 228, 229, 230, 231, 232, 233 };
    archer->animRight = { 247, 248, 249, 250, 251, 252, 253, 254, 255 };
    archer->animIdle = { 234 };
    archer->currentAnimation = archer->animIdle;
    archer->animCols = 13;
    archer->animRows = 21;
    state.enemies.push_back(archer);

    Entity* skeleton = new Entity();
    skeleton->textureID = LoadTexture("Skeleton.png");
    skeleton->entityType = ENEMY;
    skeleton->speed = 1.0f;
    skeleton->enemyType = WALKER;
    skeleton->animRight = { 143, 144, 145, 146, 147, 148, 149, 150, 151 };
    skeleton->animLeft = { 117, 118, 119, 120, 121, 122, 123, 124, 125 };
    skeleton->animIdle = { 26, 27, 28, 29, 30, 31, 32 };
    skeleton->currentAnimation = skeleton->animIdle;
    skeleton->animCols = 13;
    skeleton->animRows = 21;
    state.enemies.push_back(skeleton);

    Entity* wizard = new Entity();
    wizard->textureID = LoadTexture("Wizard.png");
    wizard->entityType = ENEMY;
    wizard->position = glm::vec3(0, -3, 0);
    wizard->animJumping = { 260, 261, 262, 263, 264, 263, 262, 261 };
    wizard->currentAnimation = wizard->animJumping;
    wizard->animCols = 13;
    wizard->animRows = 21;
    wizard->speed = 420.7135114f;     // Very important. DO NOT CHANGE!!!
    wizard->jumpPower = 3.0f;
    wizard->width = 0.5f;
    wizard->enemyType = JUMPER;
    state.enemies.push_back(wizard);

    // Initialize weapons

    Entity* spear = new Entity();
    spear->textureID = LoadTexture("dragonspear.png");
    spear->isActive = false;
    spear->acceleration = glm::vec3(0);
    spear->direction.x = -1;
    spear->height = 0.5f;
    spear->width = 0.9f;
    spear->animLeft = { 8, 9, 10, 11, 12, 13, 14, 15 };
    spear->currentAnimation = spear->animLeft;
    spear->animCols = 8;
    spear->animRows = 4;
    state.spear = spear;
    archer->weapon = spear;
    spear->position = archer->position;
}

void ProcessInput() {
    state.player->direction = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                if (state.player->collidedBottom) {
                    state.player->jump = true;
                    Mix_PlayChannel(-1, Mix_LoadWAV("JumpSound.wav"), 0);
                }
                break;
            }
            break;  // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->direction.x = -1.0f;
        state.player->currentAnimation = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->direction.x = 1.0f;
        state.player->currentAnimation = state.player->animRight;
    }

    if (glm::length(state.player->direction) > 1.0f) {
        state.player->direction = glm::normalize(state.player->direction);
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    if (!state.gameOver) {
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;
        deltaTime += accumulator;
        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }
        int enemyCount = 0;
        for (Entity* e : state.enemies) { if (e->isActive) enemyCount += 1; }
        displayText = "Enemies remaining: " + std::to_string(enemyCount);
        if (enemyCount == 0) {
            state.gameOver = true;
            displayText = " Huzzah! You won!";
        }

        while (deltaTime >= FIXED_TIMESTEP) {
            Entity* deadEnemy = state.player->AdjustCollisionsY(state.enemies);     // Player jumped on enemy, killing it.
            Entity* deadPlayer = state.player->AdjustCollisionsX(state.enemies);    // Player collided with enemy... Player is dead.
            if (deadEnemy != NULL && state.player->collidedBottom) {    // Enemy died.
                deadEnemy->isActive = false;
            }
            else if (deadPlayer != NULL || state.spear->CheckCollision(state.player)) { // Player died.
                state.gameOver = true;
                state.player->isActive = false;
                displayText = "The enemy, unfortunately, killed you.";
            }
            state.player->Update(FIXED_TIMESTEP, state.platforms, state.player);
            for (Entity* enemy : state.enemies) {
                enemy->Update(FIXED_TIMESTEP, state.platforms, state.player);
            }
            state.spear->Update(FIXED_TIMESTEP, state.platforms, state.player);
            deltaTime -= FIXED_TIMESTEP;
        }
        accumulator = deltaTime;
    }
}


// Draw all game elements on the screen
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (Entity* sign : state.signs) { sign->Render(&shaderProgram); }              // Draw signposts
    for (Entity* platform : state.platforms) { platform->Render(&shaderProgram); }  // Draw platforms
    for (Entity* enemy : state.enemies) { enemy->Render(&shaderProgram); }          // Draw enemies
    state.spear->Render(&shaderProgram);                                         // Draw spear
    state.player->Render(&shaderProgram);                                        // Draw player
    DrawText(&shaderProgram, displayText);                                       // Display current text message
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    Shutdown();
    return 0;
}
