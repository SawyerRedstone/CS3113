// CS3113 Project 1 - Simple 2D Scene

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include <SDL_mixer.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window * displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, ctgMatrix, armMatrix, OudMatrix, projectionMatrix;
Mix_Music* music;

float ctg_x = 0;
float arm_rotate = 0;

GLuint ctgTextureID;
GLuint armTextureID;
GLuint OudTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
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
    music = Mix_LoadMUS("OudMusic.wav");
    Mix_PlayMusic(music, -1);

    displayWindow = SDL_CreateWindow("Oud of this world!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    viewMatrix = glm::mat4(1.0f);
    ctgMatrix = glm::mat4(1.0f);
    armMatrix = glm::mat4(1.0f);
    OudMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    glUseProgram(program.programID);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Good setting for transparency
    ctgTextureID = LoadTexture("ctgNoArm.png");
    armTextureID = LoadTexture("arm.png");
    OudTextureID = LoadTexture("Oud.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    ctg_x += 1.0f * deltaTime;
    arm_rotate -= 90.0f * deltaTime;
    ctgMatrix = glm::mat4(1.0f);
    armMatrix = glm::mat4(1.0f);
    OudMatrix = glm::mat4(1.0f);
    OudMatrix = glm::translate(OudMatrix, glm::vec3(3.0f, 0.0f, 0.0f));


    if (ctg_x < 3) { // Walk to the oud
        ctgMatrix = glm::translate(ctgMatrix, glm::vec3(ctg_x, 0.0f, 0.0f));
        armMatrix = glm::translate(ctgMatrix, glm::vec3(0.2f, 0.2f, 0.0f));
    }
    else { // You reached the oud. Time to jam out.
        ctgMatrix = glm::translate(ctgMatrix, glm::vec3(3.0f, 0.0f, 0.0f));
        armMatrix = glm::translate(ctgMatrix, glm::vec3(0.2f, 0.2f, 0.0f));
        armMatrix = glm::rotate(armMatrix, glm::radians(arm_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    OudMatrix = glm::scale(OudMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
    armMatrix = glm::scale(armMatrix, glm::vec3(1.3f, 1.3f, 1.0f));
    ctgMatrix = glm::scale(ctgMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
}

void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetModelMatrix(ctgMatrix);
    glBindTexture(GL_TEXTURE_2D, ctgTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(OudMatrix);
    glBindTexture(GL_TEXTURE_2D, OudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(armMatrix);
    glBindTexture(GL_TEXTURE_2D, armTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
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
