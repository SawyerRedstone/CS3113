// CS3113 Project 2 - PONG

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include <string>

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, leftPaddleMatrix, rightPaddleMatrix, ballMatrix, projectionMatrix;
glm::vec3 leftPaddlePosition, leftPaddleMotion, rightPaddlePosition, rightPaddleMotion,
ballPosition, ballMotion;
float lastTicks = 0.0f;
bool gameOver = false;

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
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Let us play pong.", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    leftPaddlePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    leftPaddleMotion = glm::vec3(0.0f, 0.0f, 0.0f);

    rightPaddlePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    rightPaddleMotion = glm::vec3(0.0f, 0.0f, 0.0f);

    ballPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    ballMotion = glm::vec3(1.0f, 0.0f, 0.0f);

    viewMatrix = glm::mat4(1.0f);
    leftPaddleMatrix = glm::mat4(1.0f);
    rightPaddleMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
    leftPaddleMotion = glm::vec3(0, 0, 0);
    rightPaddleMotion = glm::vec3(0, 0, 0);

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W]) {
        leftPaddleMotion.y = 1.0f;
    }
    if (keys[SDL_SCANCODE_S]) {
        leftPaddleMotion.y = -1.0f;
    }
    if (keys[SDL_SCANCODE_UP]) {
        rightPaddleMotion.y = 1.0f;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        rightPaddleMotion.y = -1.0f;
    }

}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (size_t i = 0; i < text.size(); i++) {
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

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    float paddleSpeed = 1.5f;
    float ballSpeed = 2.0f;
    float maxY = 3.0f;
    float minY = -3.0f;

    if (gameOver == false) {
        // Update left paddle
        leftPaddlePosition += leftPaddleMotion * paddleSpeed * deltaTime;
        if (leftPaddlePosition.y < minY) {
            leftPaddlePosition.y = minY;
        }
        if (leftPaddlePosition.y > maxY) {
            leftPaddlePosition.y = maxY;
        }
        leftPaddleMatrix = glm::mat4(1.0f);
        leftPaddleMatrix = glm::translate(leftPaddleMatrix, leftPaddlePosition);

        // Update right paddle
        rightPaddlePosition += rightPaddleMotion * paddleSpeed * deltaTime;
        if (rightPaddlePosition.y < minY) {
            rightPaddlePosition.y = minY;
        }
        if (rightPaddlePosition.y > maxY) {
            rightPaddlePosition.y = maxY;
        }
        rightPaddleMatrix = glm::mat4(1.0f);
        rightPaddleMatrix = glm::translate(rightPaddleMatrix, rightPaddlePosition);

        // Update ball
        ballPosition += ballMotion * ballSpeed * deltaTime;

        if (ballPosition.y + 0.1f >= 3.75f || ballPosition.y - 0.1f <= -3.75f) {
            ballMotion.y *= -1;
        }

        if (ballPosition.x + 0.1f >= 3.9f) {
            if (ballPosition.y <= rightPaddlePosition.y + 0.75f && ballPosition.y >= rightPaddlePosition.y - 0.75f && ballMotion.x > 0)
            {
                ballMotion.x *= -1;
                ballMotion.y = (rand() % 20 - 10) * (0.1f);
            }
        }
        if (ballPosition.x - 0.1f <= -3.9f) {
            if (ballPosition.y <= leftPaddlePosition.y + 0.75f && ballPosition.y >= leftPaddlePosition.y - 0.75f && ballMotion.x < 0)
            {
                ballMotion.x *= -1;
                ballMotion.y = (rand() % 20 - 10) * (0.1f);
            }
        }
        ballMatrix = glm::mat4(1.0f);
        ballMatrix = glm::translate(ballMatrix, ballPosition);
        if (ballPosition.x - 0.1f <= -4.1f || ballPosition.x + 0.1f >= 4.1f) {
            gameOver = true;
        }
    }
}

void Draw(glm::mat4 matrix, float vertices[]) {
    program.SetModelMatrix(matrix);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(program.positionAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float leftPaddleVertices[] = { -4.1f, -0.75f, -4.1f, 0.75f, -3.9f, -0.75f, -3.9f, 0.75f };
    float rightPaddleVertices[] = { 4.1f, -0.75f, 4.1f, 0.75f, 3.9f, -0.75f, 3.9f, 0.75f };
    float ballVertices[] = { 0.1f, -0.1f, 0.1f, 0.1f, -0.1f, -0.1f, -0.1f, 0.1f };
    Draw(leftPaddleMatrix, leftPaddleVertices);
    Draw(rightPaddleMatrix, rightPaddleVertices);
    Draw(ballMatrix, ballVertices);
    //if (gameOver) { DrawText(&program, LoadTexture("font1.png"), "Game Over!", 0.5f, -0.25f, glm::vec3(-1.0f, 2.25f, 0.0f)); }
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
