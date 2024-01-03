
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./window.h"
#include "shader_program.h"
#include "vertices_data.h"
#include "shapes.h"
#include "./model.h"
#include "./camera.h"
#include "./utils.h"
#include "./textures.h"

void printGLInfo();

#define GL_CHECK_ERROR checkGLError(__LINE__)
void checkGLError(int line);

std::string readFile(const char *path);

// CONSTANTS
const char *MVP_NAME = "mvp";
const int N_ROWS = 7;
const int N_GROUPS = N_ROWS * N_ROWS;
const char *SKY_BOXES_PATHES[] = {
    "../textures/skybox/Daylight Box_Right.bmp",
    "../textures/skybox/Daylight Box_Left.bmp",
    "../textures/skybox/Daylight Box_Top.bmp",
    "../textures/skybox/Daylight Box_Bottom.bmp",
    "../textures/skybox/Daylight Box_Front.bmp",
    "../textures/skybox/Daylight Box_Back.bmp"};

// GLOBAL VARIABLES
glm::vec3 cameraPosition(-15.0f, -1.0f, 0.0f); // position initiale de la camera ou la placer
glm::vec2 cameraOrientation(0.0f, 0.0f);       // Orientation initiale (regard droit devant)
bool isFirstPersonCam = false;
glm::mat4 groupsTransform[N_GROUPS];
glm::mat4 treeTransform[N_GROUPS];
glm::mat4 rockTransform[N_GROUPS];
glm::mat4 mushroomTransform[N_GROUPS];
Camera camera(cameraPosition, cameraOrientation);
glm::vec3 groupPositions[N_GROUPS];
float grassPositions[N_ROWS * N_ROWS][2] = {};

// Define RGBA background colors
const int R = 1.0;
const int G = 1.0;
const int B = 1.0;
const int A = 0.0;

void shadersSetup(ShaderProgram &shaderProgram, std::string vertexShaderPath, std::string fragmentShaderPath)
{
    // Get shader string code
    std::string vertexShaderCode = readFile(vertexShaderPath.c_str());
    std::string fragmentShaderCode = readFile(fragmentShaderPath.c_str());

    // Create shaders
    Shader vertexShader(GL_VERTEX_SHADER, vertexShaderCode.c_str());
    Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentShaderCode.c_str());

    // Attach, link and use shaders
    shaderProgram.attachShader(vertexShader);
    shaderProgram.attachShader(fragmentShader);
    shaderProgram.link();
}

glm::mat4 getViewMatrix()
{
    if (isFirstPersonCam)
        return camera.getFirstPersonViewMatrix();
    else
        return camera.getThirdPersonViewMatrix();
}

void setPVMatrix(ShaderProgram &modelShaderProgram, glm::mat4 &projectionViewMatrix)
{
    GLint location = modelShaderProgram.getUniformLoc(MVP_NAME);
    glm::mat4 matrix = glm::mat4(1.0f) * projectionViewMatrix;
    glUniformMatrix4fv(location, 1, GL_FALSE, &projectionViewMatrix[0][0]);
}

void setCameraPerson(Window &w)
{
    if (w.getMouseScrollDirection() == 1)
        isFirstPersonCam = true;
    else if (w.getMouseScrollDirection() == -1)
        isFirstPersonCam = false;
}

void handleMouseEvent(Window &w)
{
    // Choisir la bonne caméra
    setCameraPerson(w);
    int mouseX, mouseY;
    float mouseSensitivity = 0.01f;
    w.getMouseMotion(mouseX, mouseY);
    cameraOrientation.x += static_cast<float>(mouseX) * mouseSensitivity;
    cameraOrientation.y += static_cast<float>(mouseY) * mouseSensitivity;
    // Limitez l'angle vertical
    if (cameraOrientation.y > 0.1f)
    {
        cameraOrientation.y = 0.1f;
    }
    else if (cameraOrientation.y < -0.1f)
    {
        cameraOrientation.y = -0.1f;
    }
}

void handleKeyBoardEvent(Window &w)
{
    if (w.getKeyHold(Window::Key::W) || w.getKeyPress(Window::Key::W))
    {
        cameraPosition.z -= 0.1f;
    }
    if (w.getKeyHold(Window::Key::S) || w.getKeyPress(Window::Key::S))
    {
        cameraPosition.z += 0.1f;
    }
    if (w.getKeyHold(Window::Key::A) || w.getKeyPress(Window::Key::A))
    {
        cameraPosition.x -= 0.1f;
    }
    if (w.getKeyHold(Window::Key::D) || w.getKeyPress(Window::Key::D))
    {
        cameraPosition.x += 0.1f;
    }
}

/**Take  a glm::mat4 groupsTransform where the group will be stored*/
void createTransformation()
{
    // Initialisation des matrices de transformation pour chaque groupe
    for (int row = 0; row < N_ROWS; row++)
    {
        for (int col = 0; col < N_ROWS; col++)
        {
            int groupIndex = row * N_ROWS + col;

            // Génération de la transformation aléatoire pour le groupe
            float randPosX, randPosZ;
            getGroupRandomPos(groupIndex, N_ROWS, randPosX, randPosZ);
            groupPositions[7* row + col] = glm::vec3(randPosX, 0.0f, randPosZ);
            glm::vec3 groupTranslation = glm::vec3(randPosX, -1.0f, randPosZ);
            float groupRotationY = rand01() * glm::two_pi<float>();
            float groupScale = glm::mix(0.7f, 1.3f, rand01());

            // Matrice de transformation pour le groupe
            groupsTransform[groupIndex] = glm::mat4(1.0f);
            groupsTransform[groupIndex] = glm::translate(groupsTransform[groupIndex], groupTranslation);
            groupsTransform[groupIndex] = glm::rotate(groupsTransform[groupIndex], groupRotationY, glm::vec3(0.0f, 1.0f, 0.0f));
            groupsTransform[groupIndex] = glm::scale(groupsTransform[groupIndex], glm::vec3(groupScale));

            // Matrice de transformation pour l'arbre (centré en (0, 0, 0))
            float treeScale = glm::mix(0.7f, 1.3f, rand01());
            treeTransform[groupIndex] = glm::mat4(1.0f);
            treeTransform[groupIndex] = glm::scale(treeTransform[groupIndex], glm::vec3(treeScale));

            // Matrice de transformation pour le rocher (sur un cercle autour du groupe)
            float rockRadius = glm::mix(1.0f, 2.0f, rand01());
            float rockAngle = rand01() * glm::two_pi<float>();
            rockTransform[groupIndex] = glm::mat4(1.0f);
            rockTransform[groupIndex] = glm::translate(rockTransform[groupIndex], glm::vec3(rockRadius * glm::cos(rockAngle), -0.2f, rockRadius * glm::sin(rockAngle)));
            rockTransform[groupIndex] = glm::scale(rockTransform[groupIndex], glm::vec3(0.5f));

            // Matrice de transformation pour le champignon (à la base de l'arbre)
            mushroomTransform[groupIndex] = glm::mat4(1.0f);
            mushroomTransform[groupIndex] = glm::translate(mushroomTransform[groupIndex], glm::vec3(0.3f * treeScale, 0.0f, 0.3f * treeScale));
            mushroomTransform[groupIndex] = glm::scale(mushroomTransform[groupIndex], glm::vec3(0.05f));

            // Generate random positions for grass
            float randomX = -25.0f + rand01() * 50.0f;
            float randomZ = -25.0f + rand01() * 50.0f;
            grassPositions[row][0] = randomX;
            grassPositions[row][1] = randomZ;
        }
    }
}

void drawWorld(ShaderProgram &modelShaderProgram, ShaderProgram& hudShaderProgram, glm::mat4 &projectionViewMatrix)
{
    // Création des models
    Model mushroomModel("../models/mushroom.obj");
    Model rockModel("../models/rock.obj");
    Model suzanneModel("../models/suzanne.obj");
    Model treeModel("../models/tree.obj");

    // Textures
    Texture2D suzanneTexture("../models/suzanneTexture.png", GL_CLAMP_TO_EDGE);
    suzanneTexture.enableMipmap();
    Texture2D treeTexture("../models/treeTexture.png", GL_CLAMP_TO_EDGE);
    treeTexture.enableMipmap();
    Texture2D treeBillboardTexture("../textures/treeBillboard.png", GL_CLAMP_TO_EDGE);
    treeBillboardTexture.enableMipmap();
    Texture2D rockTexture("../models/rockTexture.png", GL_CLAMP_TO_EDGE);
    rockTexture.enableMipmap();
    Texture2D mushroomTexture("../models/mushroomTexture.png", GL_CLAMP_TO_EDGE);
    mushroomTexture.enableMipmap();

    for (size_t i = 0; i < N_GROUPS; i++)
    {
        modelShaderProgram.use();
        GLint location = modelShaderProgram.getUniformLoc(MVP_NAME);
        float distance = abs(glm::length(cameraPosition-groupPositions[i]));
        if(distance <= 25){
        glm::mat4 treeMatrix = projectionViewMatrix * groupsTransform[i] * treeTransform[i];
        glUniformMatrix4fv(location, 1, GL_FALSE, &treeMatrix[0][0]);
        treeTexture.use();
        treeModel.draw();
        treeTexture.unuse();
        }
        else {
            // Dessiner une image 2D des arbres lointains
            hudShaderProgram.use();
            GLfloat billboardVertices[] = {
                groupPositions[i].x, groupPositions[i].y, groupPositions[i].z,0.0f, 0.0f,
                groupPositions[i].x, groupPositions[i].y + 3.47f, groupPositions[i].z,0.0f, 1.0f,
                groupPositions[i].x + 0.96f*3.47f, groupPositions[i].y+ 3.47f, groupPositions[i].z,1.0f, 1.0f,
                groupPositions[i].x + 0.96f * 3.47f, groupPositions[i].y, groupPositions[i].z,1.0f, 0.0f
            };
            BasicShapeElements billboard(billboardVertices, sizeof(billboardVertices), squarePlaneIndices, sizeof(squarePlaneIndices));
            billboard.enableAttribute(0, 3, 5 * sizeof(float), 0);
            billboard.enableAttribute(1, 2, 5 * sizeof(float), 3);
            glm::mat4 billboardTreeMatrix = projectionViewMatrix * treeTransform[i];
            billboardTreeMatrix = glm::translate(billboardTreeMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
            glUniformMatrix4fv(location, 1, GL_FALSE, &billboardTreeMatrix[0][0]);
            treeBillboardTexture.use();
            billboard.draw(GL_TRIANGLES, 6);
            treeBillboardTexture.unuse();
        }
        modelShaderProgram.use();

        glm::mat4 rockMatrix = projectionViewMatrix * groupsTransform[i] * rockTransform[i];
        glUniformMatrix4fv(location, 1, GL_FALSE, &rockMatrix[0][0]);
        rockTexture.use();
        rockModel.draw();
        rockTexture.unuse();

        glm::mat4 mushroomMatrix = projectionViewMatrix * groupsTransform[i] * mushroomTransform[i];
        glUniformMatrix4fv(location, 1, GL_FALSE, &mushroomMatrix[0][0]);
        mushroomTexture.use();
        mushroomModel.draw();
        mushroomTexture.unuse();
    }

    GLint location = modelShaderProgram.getUniformLoc(MVP_NAME);
    glm::mat4 suzanneMatrix;
    if (!isFirstPersonCam)
    {
        suzanneMatrix = projectionViewMatrix * glm::translate(glm::mat4(1.0f), cameraPosition);
        suzanneMatrix = glm::scale(suzanneMatrix, glm::vec3(0.5f));

        glUniformMatrix4fv(location, 1, GL_FALSE, &suzanneMatrix[0][0]);
        suzanneTexture.use();
        suzanneModel.draw();
        suzanneTexture.unuse();
    }
}

void drawSky(ShaderProgram &skyBoxlShaderProgram, glm::mat4 &projectionMatrix)
{
    TextureCubeMap cubeMapTexture(SKY_BOXES_PATHES);
    // Créer le skybox
    BasicShapeArrays skyBox(skyboxVertices, sizeof(skyboxVertices));
    skyBox.enableAttribute(0, 3, 0, 0);
    // Sauvegardez l'ancienne fonction du test de profondeur
    GLenum oldDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, (GLint *)&oldDepthFunc);
    glDepthFunc(GL_GEQUAL);
    glDisable(GL_DEPTH_TEST);
    skyBoxlShaderProgram.use();
    glm::mat4 viewMatrixNoTranslation = glm::mat4(glm::mat3(getViewMatrix()));
    glm::mat4 skymvp = projectionMatrix * viewMatrixNoTranslation * glm::mat4(1.0f);
    glUniformMatrix4fv(skyBoxlShaderProgram.getUniformLoc(MVP_NAME), 1, GL_FALSE, &skymvp[0][0]);
    cubeMapTexture.use();
    skyBox.draw(GL_TRIANGLES, 36);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(oldDepthFunc);
}

void drawHud(ShaderProgram &hudShaderProgram, Texture2D &HudTexture)
{
    BasicShapeElements hud(hudSquare, sizeof(hudSquare), squarePlaneIndices, sizeof(squarePlaneIndices));
    hud.enableAttribute(0, 3, 5 * sizeof(float), 0);
    hud.enableAttribute(1, 2, 5 * sizeof(float), 3);
    // Configurer le shader et la projection
    hudShaderProgram.use();
    GLint location = hudShaderProgram.getUniformLoc(MVP_NAME);
    // attributs de projection
    float halfHeight = 15.0f;
    float halfWidth = 15.0f;
    glm::mat4 projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
    glm::mat4 hudmvp = glm::translate(projection, glm::vec3(-13.0f, -13.0f, 0.0f));
    glUniformMatrix4fv(location, 1, GL_FALSE, &hudmvp[0][0]);
    HudTexture.use();
    hud.draw(GL_TRIANGLES, 6);
    HudTexture.unuse();
}

void drawGrass(ShaderProgram &shaderProgram, Texture2D &texture, glm::mat4& projectionViewMatrix)
{
    shaderProgram.use();
    GLint location = shaderProgram.getUniformLoc(MVP_NAME);
    for (size_t i = 0; i < N_ROWS * N_ROWS; i++)
    {
        BasicShapeElements grass = BasicShapeElements(normalGrassSquare, sizeof(normalGrassSquare), squarePlaneIndices, sizeof(squarePlaneIndices));
                      
        grass.enableAttribute(0, 3, 5 * sizeof(float), 0);
        grass.enableAttribute(1, 2, 5 * sizeof(float), 3);

        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(grassPositions[i][0], 0.0, grassPositions[i][1]));
        glm::mat4 mvp = projectionViewMatrix * model;
        glUniformMatrix4fv(location, 1, GL_FALSE, &mvp[0][0]);
        texture.use();
        grass.draw(GL_TRIANGLES, 6);
        texture.unuse();
    }
}

int main(int argc, char *argv[])
{
    Window w;
    if (!w.init())
        return -1;

    GLenum rev = glewInit();
    if (rev != GLEW_OK)
    {
        std::cout << "Could not initialize glew! GLEW_Error: " << glewGetErrorString(rev) << std::endl;
        return -2;
    }

    float ratio = (float)w.getWidth() / (float)w.getHeight();
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(70.0f), ratio, 0.1f, 200.0f);

    // Créez une instance de BasicShapeElements en utilisant les données du plan carré
    BasicShapeElements squarePlane(squarePlaneVertices, sizeof(squarePlaneVertices), squarePlaneIndices, sizeof(squarePlaneIndices));
    squarePlane.enableAttribute(0, 3, 0, 0);
    squarePlane.enableAttribute(1, 2, 0, 0);
    // Créer un rectangle
    BasicShapeElements rectanglePlane(rectangleVertices, sizeof(rectangleVertices), squarePlaneIndices, sizeof(squarePlaneIndices));
    rectanglePlane.enableAttribute(0, 3, 0, 0);
    rectanglePlane.enableAttribute(1, 2, 0, 0);

    // Créer les transformations
    createTransformation();

    // Créer les textures
    Texture2D floorTexture("../textures/groundSeamless.jpg", GL_REPEAT);
    floorTexture.enableMipmap();
    Texture2D riverTexture("../textures/waterSeamless.jpg", GL_REPEAT);
    riverTexture.enableMipmap();
    Texture2D HudTexture("../textures/heart.png", GL_CLAMP_TO_EDGE);
    HudTexture.enableMipmap();
    Texture2D grassTexture("../textures/grassAtlas.png", GL_CLAMP_TO_EDGE);
    grassTexture.enableMipmap();

    // Shader program du modèle.
    ShaderProgram modelShaderProgram;
    shadersSetup(modelShaderProgram, "shaders/model.vs.glsl", "shaders/model.fs.glsl");
    // Shader program de la rivière.
    ShaderProgram riverShaderProgram;
    shadersSetup(riverShaderProgram, "shaders/river.vs.glsl", "shaders/river.fs.glsl");
    // Shader program du skybox
    ShaderProgram skyBoxlShaderProgram;
    shadersSetup(skyBoxlShaderProgram, "shaders/skybox.vs.glsl", "shaders/skybox.fs.glsl");
    // Shader program du hud
    ShaderProgram hudShaderProgram;
    shadersSetup(hudShaderProgram, "shaders/hud.vs.glsl", "shaders/hud.fs.glsl");
    // Shader program du square
    ShaderProgram squareShaderProgram;
    shadersSetup(squareShaderProgram, "shaders/square.vs.glsl", "shaders/square.fs.glsl");

    // Couleur de fond blanche
    glClearColor(R, G, B, A);

    // Activer le depth test.
    glEnable(GL_DEPTH_TEST);

    int selectShape = 0;
    bool isRunning = true;

    while (isRunning)
    {
        if (w.shouldResize())
            glViewport(0, 0, w.getWidth(), w.getHeight());
        glm::mat4 projectionViewMatrix = projectionMatrix * getViewMatrix();

        // update camera orientation
        handleMouseEvent(w);
        handleKeyBoardEvent(w);
        // TODO Partie 1: Nettoyer les tampons appropriées.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky(skyBoxlShaderProgram, projectionMatrix);
        modelShaderProgram.use();
        drawWorld(modelShaderProgram, hudShaderProgram, projectionViewMatrix);
        setPVMatrix(modelShaderProgram, projectionViewMatrix);

        modelShaderProgram.use();

        floorTexture.use();
        squarePlane.draw(GL_TRIANGLES, 6);
        floorTexture.unuse();

        // Draw river
        riverShaderProgram.use();
        float currentTime = w.getTick() / 1000.0f;
        GLint riverlocation = riverShaderProgram.getUniformLoc(MVP_NAME);
        GLint timeLocation = riverShaderProgram.getUniformLoc("time");
        glUniform1f(timeLocation, currentTime);
        riverTexture.use();
        glUniformMatrix4fv(riverlocation, 1, GL_FALSE, &projectionViewMatrix[0][0]);
        rectanglePlane.draw(GL_TRIANGLES, 6);
        riverTexture.unuse();

        drawHud(hudShaderProgram, HudTexture);
        drawGrass(hudShaderProgram, grassTexture, projectionViewMatrix);

        w.swap();
        w.pollEvent();
        isRunning = !w.shouldClose() && !w.getKeyPress(Window::Key::ESC);
    }

    return 0;
}

void checkGLError(int line)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "Line " << line << ": ";
        switch (error)
        {
        case GL_INVALID_ENUM:
            std::cerr << "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            std::cerr << "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << "GL_STACK_UNDERFLOW";
            break;
        case GL_STACK_OVERFLOW:
            std::cerr << "GL_STACK_OVERFLOW";
            break;
        default:
            std::cerr << "Unknown gl error occured!";
            break;
        }
        std::cerr << std::endl;
    }
}

void printGLInfo()
{
    std::cout << "OpenGL info:" << std::endl;
    std::cout << "    Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "    Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "    Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "    Shading version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

std::string readFile(const char *path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
