// Sawyer Redstone
// Platformer Game
// CS3113 Project #5

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

#include "Scene.h"
#include "MenuScreen.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "WinScreen.h"
#include "Entity.h"
#include "Map.h"
#include "GameUtil.h"
#include <vector>

#include <sstream> // allows for formatting float strings with variable number of decimal places
#include <iomanip> // related to above

//Global variables
bool gameOver = false;
int score = 0;
int lives = 3;
SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram shaderProgram;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
std::string displayText;

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
Scene* currentScene;
Scene* sceneList[5];

// Switch between game scenes
void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

// Initialize all game elements
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Platformer Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    shaderProgram.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    shaderProgram.SetViewMatrix(viewMatrix);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    shaderProgram.SetProjectionMatrix(projectionMatrix);
    modelMatrix = glm::mat4(1.0f);
    glUseProgram(shaderProgram.programID);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    sceneList[0] = new MenuScreen();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new WinScreen();
    SwitchToScene(sceneList[0]);
}


void ProcessInput() {

    currentScene->state.player->direction = glm::vec3(0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                if (currentScene == sceneList[0]) { currentScene->state.nextScene = 1; }
                break;
            case SDLK_SPACE:
                if (currentScene == sceneList[2]) {                                // Special treatment for jumping in the underwater level. 
                    currentScene->state.player->jump = true;
                }
                else if (currentScene->state.player->collidedBottom) {
                    currentScene->state.player->jump = true;
                    Mix_PlayChannel(-1, Mix_LoadWAV(".\\Assets\\JumpSound.wav"), 0);
                }
                break;
            case SDLK_q:
                gameIsRunning = false;
                break;
            }
            break;  // SDL_KEYDOWN
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->direction.x = -1.0f;
        currentScene->state.player->currentAnimation = currentScene->state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->direction.x = 1.0f;
        currentScene->state.player->currentAnimation = currentScene->state.player->animRight;
    }
    // Following code allows for up and down motion for debugging
    //else if (keys[SDL_SCANCODE_UP]) {
    //    currentScene->state.player->direction.y = 1.0f;
    //}
    //else if (keys[SDL_SCANCODE_DOWN]) {
    //    currentScene->state.player->direction.y = -1.0f;
    //}
    else {
        currentScene->state.player->animIndex = 0;
        currentScene->state.player->animIdle = { currentScene->state.player->currentAnimation[0] };
        currentScene->state.player->currentAnimation = currentScene->state.player->animIdle;
    }
    if (glm::length(currentScene->state.player->direction) > 1.0f) {
        currentScene->state.player->direction = glm::normalize(currentScene->state.player->direction);
    }
}



void Update() {
    if (gameOver) {
        return;
    }

    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Enemy dies if jumped upon, except in underwater level (where enemies cannot die) 
        Entity* deadEnemy = NULL;
        if (currentScene != sceneList[2]) {
            deadEnemy = currentScene->state.player->AdjustCollisionsY(currentScene->state.enemies);
            if (deadEnemy) {
                deadEnemy->isActive = false;
                if (deadEnemy->weapon != NULL) {
                    deadEnemy->weapon->isActive = false;
                }
            }
        }

        bool deadPlayer = false;

        // Player dies if collides with enemy from any direction in underwater level. 
        if ((currentScene == sceneList[2]) &&
            (currentScene->state.player->AdjustCollisionsY(currentScene->state.enemies) ||
                currentScene->state.player->AdjustCollisionsX(currentScene->state.enemies))) {
            deadPlayer = true;
            Mix_PlayChannel(-1, Mix_LoadWAV(".\\Assets\\UnderwaterDeath.wav"), 0);
        }
        // On all other levels, player dies if hits enemy's side
        else if (currentScene->state.player->AdjustCollisionsX(currentScene->state.enemies)) {
            deadPlayer = true;
            Mix_PlayChannel(-1, Mix_LoadWAV(".\\Assets\\PlayerDies2.wav"), 0);
        }
        // Player gets hit by weapon and dies
        else if (currentScene->state.weapon && currentScene->state.player->CheckCollision(currentScene->state.weapon)) {
            deadPlayer = true;
            Mix_PlayChannel(-1, Mix_LoadWAV(".\\Assets\\PlayerDies2.wav"), 0);
        }
        // Player dies if falls in a bottomless pit
        else if (currentScene->state.player->position.y < -10) {
            deadPlayer = true;
            Mix_PlayChannel(-1, Mix_LoadWAV(".\\Assets\\FallingSound.wav"), 0);
        }
        if (deadPlayer) {
            currentScene->state.player->position = currentScene->state.player->startingPosition;   // If killed, return to level's starting pos.
            lives -= 1;
            if (lives == 0) {
                currentScene->state.player->isActive = false;
                gameOver = true;
            }
        }

        for (Entity* coin : currentScene->state.coins) {
            if (currentScene->state.player->CheckCollision(coin)) {
                coin->isActive = false;
                Mix_PlayChannel(-1, Mix_LoadWAV(".\\Assets\\Pickup_Coin4.wav"), 0);
                score += 10;
                std::cout << score << " ";
            }
        }
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75f, 0));
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75f, 0));
    }
}


// Draw all game elements on the screen
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram.SetViewMatrix(viewMatrix);
    currentScene->Render(&shaderProgram);

    // Show updated game status (lives, score) on top of screen
    GLuint fontTextureID = GameUtil::LoadTexture(".\\Assets\\font1.png");
    if (gameOver) {
        GameUtil::DrawText(&shaderProgram, fontTextureID, "You lose!", 0.6f, -0.25f, glm::vec3(1.2, -4, 0));
    }
    else if (currentScene != sceneList[0] && currentScene != sceneList[4]) {  // Show score at all levels except intro and end screen
        if (currentScene->state.player->position.x <= 5) {
            GameUtil::DrawText(&shaderProgram, fontTextureID, "Score: " + std::to_string(score) + ", Lives: " + std::to_string(lives), 0.4f, -0.20f, glm::vec3(1, -1, 0));
        }
        else {
            GameUtil::DrawText(&shaderProgram, fontTextureID, "Score: " + std::to_string(score) + ", Lives: " + std::to_string(lives), 0.4f, -0.20f, glm::vec3(currentScene->state.player->position.x - 4, -1, 0));
        }
    }
    else if (currentScene == sceneList[4]) {
        GameUtil::DrawText(&shaderProgram, fontTextureID, "Score: " + std::to_string(score), 0.6f, -0.25f, glm::vec3(2.2, -4.5, 0));
    }


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
        if (currentScene->state.nextScene >= 0) { SwitchToScene(sceneList[currentScene->state.nextScene]); }
        Render();
    }
    Shutdown();
    return 0;
}
