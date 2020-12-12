#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Scene.h"
#include "MenuScreen.h"
#include "Level1.h"
#include "WinScreen.h"
#include "LoseScreen.h"
#include "Entity.h"
#include "Util.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint fontTextureID;
GLuint heartTextureID;

GameState state;
Scene* currentScene;
Scene* sceneList[4];

// Switch between game scenes
void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("STAR SHOOTER!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 720);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    fontTextureID = Util::LoadTexture("font1.png");
    heartTextureID = Util::LoadTexture("heart.png");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    sceneList[0] = new MenuScreen();
    sceneList[1] = new Level1();
    sceneList[2] = new WinScreen();
    sceneList[3] = new LoseScreen();
    SwitchToScene(sceneList[0]);

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    state.music = Mix_LoadMUS("space.mp3");
    Mix_PlayMusic(state.music, -1);
}

void ProcessInput() {
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
                    // if (currentScene == sceneList[2] || currentScene == sceneList[3]) { currentScene->state.nextScene = 0; }
                    break;
                    case SDLK_SPACE:
                        // Shoot
                        if (currentScene->state.shooting == false) { 
                            currentScene->state.shooting = true;
                            Mix_PlayChannel(-1, Mix_LoadWAV("shoot.wav"), 0);
                        }
                        break; 
                }
                break;
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_A]) {
        //state.player->rotation.y += 1.0f;     // Professor Carmine version
        currentScene->state.player->rotation.y += 0.2f;
    }
    else if (keys[SDL_SCANCODE_D]) {
        //state.player->rotation.y -= 1.0f;
        currentScene->state.player->rotation.y -= 0.2f;
    }
    currentScene->state.player->velocity.x = 0;
    currentScene->state.player->velocity.z = 0;

    if (keys[SDL_SCANCODE_W]) {
        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * -2.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * 2.0f;
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix,
        glm::radians(currentScene->state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -currentScene->state.player->position);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);

    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    if (currentScene == sceneList[0]) {
        Util::DrawText(&program, fontTextureID, "STAR SHOOTER!", 0.7, -0.25f, glm::vec3(-2, 3.2, 0));
        Util::DrawText(&program, fontTextureID, "Collect all diamonds.", 0.5, -0.25f, glm::vec3(-6, 2.2, 0));
        Util::DrawText(&program, fontTextureID, "Don't touch UFOs!", 0.5, -0.25f, glm::vec3(-6, 1.2, 0));
        Util::DrawText(&program, fontTextureID, "Press SPACE to shoot, A S D W to move.", 0.5, -0.25f, glm::vec3(-6, 0.2, 0));
        Util::DrawText(&program, fontTextureID, "Press ENTER to play!", 0.7, -0.30f, glm::vec3(-6, -1.2, 0));
    }
    if (currentScene == sceneList[1]) { 
        // std::string lives = "Lives: " + std::to_string(currentScene->state.player->lives);
        std::string diamonds = "Diamonds Remaining: " + std::to_string(currentScene->state.player->remainingCoins);
        Util::DrawText(&program, fontTextureID, diamonds, 0.5, -0.25f, glm::vec3(-6, 3.2, 0));

        for (int i = 0; i < currentScene->state.player->lives; i++) {
            // These icons are small, so just move 0.5 to the right for each one.
            Util::DrawIcon(&program, heartTextureID, glm::vec3(5 + (i * 0.5f), 3.2, 0));
        }
    }
    if (currentScene == sceneList[2]) {
        Util::DrawText(&program, fontTextureID, "You win!", 0.7, -0.25f, glm::vec3(-2, 3.2, 0));
        //Util::DrawText(&program, fontTextureID, "Press ENTER to play again!", 0.7, -0.30f, glm::vec3(-5, 2.2, 0));
    }
    if (currentScene == sceneList[3]) {
        Util::DrawText(&program, fontTextureID, "Unfortunately, you lost.", 0.7, -0.25f, glm::vec3(-5.25, 0.2, 0));
        //Util::DrawText(&program, fontTextureID, "Press ENTER to try again!", 0.7, -0.30f, glm::vec3(-5, -1.2, 0));
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
