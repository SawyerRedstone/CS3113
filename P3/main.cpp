// Piggy Banker (Lunar Lander clone)
// CS3313 Project #3
// By Sawyer Redstone


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
#include <string>

SDL_Window* displayWindow;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
Mix_Music* music;

bool gameIsRunning = true;
bool gameOver = false;
bool gameWon = false;

Player* player;
std::vector<Entity*> objects; // Non-player objects, including walls, landing pad, etc.

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

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("SillyIntroMusic.mp3");
    Mix_PlayMusic(music, -1);

    displayWindow = SDL_CreateWindow("Piggy Banker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    glUseProgram(program.programID);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize Player
    player = new Player();
    player->position = glm::vec3(0.0f, 3.5f, 0.0f);
    player->movement = glm::vec3(0);
    player->acceleration = glm::vec3(0, -0.0981f, 0);
    player->speed = 1.0f;
    player->textureID = LoadTexture("Coin.png");
    player->height = 0.8f;
    player->width = 0.6f;

    // Initialize walls and other non-player stuff
    glm::vec3 wallPositions[] = {
        // Left side
        glm::vec3(-4.5f, 3.25f, 0.0f),  glm::vec3(-4.5f, 2.25f, 0.0f),
        glm::vec3(-4.5f, 1.25f, 0.0f),  glm::vec3(-4.5f, 0.25f, 0.0f),
        glm::vec3(-4.5f, -0.75f, 0.0f),  glm::vec3(-4.5f, -1.75f, 0.0f),
        glm::vec3(-4.5f, -2.75f, 0.0f), glm::vec3(-4.5f, -3.25f, 0.0f),
        // Right side
        glm::vec3(4.5f, 3.25f, 0.0f),  glm::vec3(4.5f, 2.25f, 0.0f),
        glm::vec3(4.5f, 1.25f, 0.0f),  glm::vec3(4.5f, 0.25f, 0.0f),
        glm::vec3(4.5f, -0.75f, 0.0f),  glm::vec3(4.5f, -1.75f, 0.0f),
        glm::vec3(4.5f, -2.75f, 0.0f), glm::vec3(4.5f, -3.25f, 0.0f),
        // Bottom
        glm::vec3(-3.5f, -3.25f, 0.0f),  glm::vec3(-2.5, -3.25f, 0.0f),
        glm::vec3(0.5f, -3.25f, 0.0f),  glm::vec3(1.5f, -3.25f, 0.0f),
        glm::vec3(2.5f, -3.25f, 0.0f), glm::vec3(3.5f, -3.25f, 0.0f),
        // Other
        glm::vec3(3.5f, 1.25f, 0.0f), glm::vec3(2.5f, 1.25f, 0.0f),
        glm::vec3(-2.5f, -0.25f, 0.0f), glm::vec3(-1.5, -0.25f, 0.0f),
        glm::vec3(-0.5f, -0.25f, 0.0f)
    };

    for (glm::vec3 position : wallPositions) {
        Entity* wall = new Entity(WALL);
        wall->position = position;
        wall->textureID = LoadTexture("Tile.png");
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        wall->modelMatrix = glm::translate(modelMatrix, position);
        objects.push_back(wall);
    }

    // Add piggybanks (landing pads) 
    glm::vec3 piggyPositions[] = { glm::vec3(-1.5f, -3.25f, 0.0f), glm::vec3(-0.5f, -3.25f, 0.0f) };
    for (glm::vec3 position : piggyPositions) {
        Entity* landingPad = new Entity(LANDINGPAD);
        landingPad->position = position;
        landingPad->textureID = LoadTexture("Piggybank.png");
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        landingPad->modelMatrix = glm::translate(modelMatrix, position);
        objects.push_back(landingPad);
    }
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        }
    }
    player->movement = glm::vec3(0);
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        player->movement.x = -1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        player->movement.x = 1.0f;
    }
    if (glm::length(player->movement) > 1.0f) {
        player->movement = glm::normalize(player->movement);
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    if (!gameOver) {
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;
        deltaTime += accumulator;

        if (deltaTime < FIXED_TIMESTEP) {
            accumulator = deltaTime;
            return;
        }
        while (deltaTime >= FIXED_TIMESTEP) {
            deltaTime -= FIXED_TIMESTEP;

            // Update the player's position and motion
            player->Update(FIXED_TIMESTEP);

            // Then check if it hit something (thereby winning or losing)
            Entity* bumped = player->CheckCollision(objects);
            if (bumped != NULL) {
                if (bumped->type == WALL) {    // Player loses
                    gameWon = false;
                }
                if (bumped->type == LANDINGPAD) {  // Player wins
                    gameWon = true;
                }
                gameOver = true;
                player->acceleration.x = 0; player->acceleration.y = 0; player->velocity.y = 0; player->speed = 0;
            }
        }
        accumulator = deltaTime;
    }
}


void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void showResults() {
    GLuint fontTextureID = LoadTexture("font1.png");
    if (gameWon) {
        DrawText(&program, fontTextureID, "Mission Successful!", 0.5f, -0.25f, glm::vec3(-1.0f, 2.25f, 0.0f));
    }
    else {
        DrawText(&program, fontTextureID, "Mission Failed!", 0.5f, -0.25f, glm::vec3(-1.0f, 2.25f, 0.0f));
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    player->Render(&program);
    for (Entity* object : objects) {
        object->Render(&program);
    }
    if (gameOver) {
        showResults();
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
        Render();
    }
    Shutdown();
    return 0;
}
