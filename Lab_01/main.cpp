
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"
#include "shader_program.h"
#include "vertices_data.h"
#include "shapes.h"

void printGLInfo();

#define GL_CHECK_ERROR checkGLError(__LINE__)
void checkGLError(int line);

void changeRGB(GLfloat *color);
void changePos(GLfloat *pos, float &cx, float &cy, float &dx, float &dy);

std::string readFile(const char *path);

// CONSTANTS
const int POSITION_ATTRIBUT_INDEX = 0, COLOR_ATTRIBUT_INDEX = 1;
const int POSITION_ATTRIBUT_OFFSET = 0, COLOR_ATTRIBUT_OFFSET = 3;
const int THREE_COMPONENTS = 3, SIX_COMPONENTS = 6;
const char *MVP_NAME = "mvp";

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

void calculateMVP(float angleDeg, float windowWidth, float windowHeight, GLint mvpLocation)
{
    // Utiliser glm pour les calculs de matrices.
    glm::mat4 matrix;
    // Initialiser les matrices modele, de vue et de projection à l'identité
    glm::mat4 modelMatrix(1.0);
    glm::mat4 viewMatrix(1.0);
    glm::mat4 projectionMatrix(1.0);
    glm::vec3 observation(0.0, 0.5, 2);

    modelMatrix = glm::rotate(modelMatrix, GLfloat(glm::radians(angleDeg)), glm::vec3(0.1, 1.0, 0.1));
    viewMatrix = glm::lookAt(observation, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));

    float ratio = windowWidth / windowHeight;
    projectionMatrix = glm::perspective(glm::radians(70.0f), ratio, 0.1f, 10.0f);

    matrix = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

int main()
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

    // TODO Partie 1: Instancier les shader programs ici.
    // ... basic;
    ShaderProgram basicShaderProgram;
    shadersSetup(basicShaderProgram, "shaders/basic.vs.glsl", "shaders/basic.fs.glsl");

    // ... color;
    ShaderProgram colorShaderProgram;
    shadersSetup(colorShaderProgram, "shaders/color.vs.glsl", "shaders/color.fs.glsl");

    // TODO Partie 2: Shader program de transformation.
    // ... transform;
    ShaderProgram transformShaderProgram;
    shadersSetup(transformShaderProgram, "shaders/transform.vs.glsl", "shaders/transform.fs.glsl");
    // ... location;
    GLint mvpLocation = transformShaderProgram.getUniformLoc(MVP_NAME);

    // Variables pour la mise à jour, ne pas modifier.
    float cx = 0, cy = 0;
    float dx = 0.019;
    float dy = 0.0128;

    float angleDeg = 0.0f;

    // Tableau non constant de la couleur
    GLfloat onlyColorTriVertices[] = {
        // TODO Partie 1: Rempliser adéquatement le tableau.
        // Vous pouvez expérimenter avec une couleur uniforme
        // de votre choix ou plusieurs différentes en chaque points.
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f};

    // TODO Partie 1: Instancier vos formes ici.
    BasicShapeArrays firstTriangle(triVertices, sizeof(triVertices));
    firstTriangle.enableAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, THREE_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);

    BasicShapeArrays firstSquare(squareVertices, sizeof(squareVertices));
    firstSquare.enableAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, THREE_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);

    BasicShapeArrays colorTriangle(colorTriVertices, sizeof(colorTriVertices));
    colorTriangle.enableAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);
    colorTriangle.enableAttribute(COLOR_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), COLOR_ATTRIBUT_OFFSET);

    BasicShapeArrays colorSquare(colorSquareVertices, sizeof(colorSquareVertices));
    colorSquare.enableAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);
    colorSquare.enableAttribute(COLOR_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), COLOR_ATTRIBUT_OFFSET);

    BasicShapeMultipleArrays multiArrayTriangle(triVertices, sizeof(triVertices), onlyColorTriVertices, sizeof(onlyColorTriVertices));
    multiArrayTriangle.enablePosAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, THREE_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);
    multiArrayTriangle.enableColorAttribute(COLOR_ATTRIBUT_INDEX, THREE_COMPONENTS, THREE_COMPONENTS * sizeof(float), COLOR_ATTRIBUT_OFFSET);

    BasicShapeElements elementsSquare(colorSquareVerticesReduced, sizeof(colorSquareVerticesReduced), indexes, sizeof(indexes));
    elementsSquare.enableAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);
    elementsSquare.enableAttribute(COLOR_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), COLOR_ATTRIBUT_OFFSET);

    // TODO Partie 2: Instancier le cube ici.
    // ...
    BasicShapeElements cube(cubeVertices, sizeof(cubeVertices), cubeIndexes, sizeof(cubeIndexes));
    cube.enableAttribute(POSITION_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), POSITION_ATTRIBUT_OFFSET);
    cube.enableAttribute(COLOR_ATTRIBUT_INDEX, THREE_COMPONENTS, SIX_COMPONENTS * sizeof(float), COLOR_ATTRIBUT_OFFSET);

    // TODO Partie 1: Donner une couleur de remplissage aux fonds.
    // Couleur de fond blanche
    glClearColor(R, G, B, A);

    // TODO Partie 2: Activer le depth test.
    glEnable(GL_DEPTH_TEST);

    int selectShape = 0;
    bool isRunning = true;

    while (isRunning)
    {
        if (w.shouldResize())
            glViewport(0, 0, w.getWidth(), w.getHeight());

        // TODO Partie 1: Nettoyer les tampons appropriées.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (w.getKey(Window::Key::T))
        {
            selectShape = ++selectShape < 7 ? selectShape : 0;
            std::cout << "Selected shape: " << selectShape << std::endl;
        }

        // TODO Partie 1: Mise à jour des données du triangle
        changeRGB(&onlyColorTriVertices[0]);
        changeRGB(&onlyColorTriVertices[3]);
        changeRGB(&onlyColorTriVertices[6]);

        GLfloat *multiArrayTrianglePosition = multiArrayTriangle.mapPosData();
        changePos(multiArrayTrianglePosition, cx, cy, dx, dy);
        multiArrayTriangle.unmapPosData();
        multiArrayTriangle.updateColorData(onlyColorTriVertices, sizeof(onlyColorTriVertices));

        // TODO Partie 1: Utiliser le bon shader programme selon la forme.
        // N'hésiter pas à utiliser le fallthrough du switch case.
        switch (selectShape)
        {
        case 0:
        case 1:
            basicShaderProgram.use();
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            colorShaderProgram.use();
            break;
        case 6:
            transformShaderProgram.use();
            break;
        default:
            basicShaderProgram.use();
        }

        // TODO Partie 2: Calcul des matrices et envoyer une matrice résultante mvp au shader.
        if (selectShape == 6)
        {
            angleDeg += 0.5f;
            calculateMVP(angleDeg, (float)w.getWidth(), (float)w.getHeight(), mvpLocation);
        }

        // TODO Partie 1: Dessiner la forme sélectionnée.
        switch (selectShape)
        {
        case 0:
            firstTriangle.draw(GL_TRIANGLES, 3);
            break;

        case 1:
            firstSquare.draw(GL_TRIANGLE_STRIP, 4);
            break;

        case 2:
            colorTriangle.draw(GL_TRIANGLES, 3);
            break;

        case 3:
            colorSquare.draw(GL_TRIANGLE_STRIP, 4);
            break;

        case 4:
            multiArrayTriangle.draw(GL_TRIANGLES, 3);
            break;

        case 5:
            elementsSquare.draw(GL_TRIANGLES, 6);
            break;

        case 6:
            cube.draw(GL_TRIANGLES, 36);
            break;

        default:
            firstTriangle.draw(GL_TRIANGLES, 3);
        }

        w.swap();
        w.pollEvent();
        isRunning = !w.shouldClose() && !w.getKey(Window::Key::ESC);
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

void changeRGB(GLfloat *color)
{
    unsigned char r = color[0] * 255;
    unsigned char g = color[1] * 255;
    unsigned char b = color[2] * 255;

    if (r > 0 && b == 0)
    {
        r--;
        g++;
    }
    if (g > 0 && r == 0)
    {
        g--;
        b++;
    }
    if (b > 0 && g == 0)
    {
        r++;
        b--;
    }
    color[0] = r / 255.0f;
    color[1] = g / 255.0f;
    color[2] = b / 255.0f;
}

void changePos(GLfloat *pos, float &cx, float &cy, float &dx, float &dy)
{
    if ((cx < -1 && dx < 0) || (cx > 1 && dx > 0))
        dx = -dx;
    pos[0] += dx;
    pos[3] += dx;
    pos[6] += dx;
    cx += dx;
    if ((cy < -1 && dy < 0) || (cy > 1 && dy > 0))
        dy = -dy;
    pos[1] += dy;
    pos[4] += dy;
    pos[7] += dy;
    cy += dy;
}

std::string readFile(const char *path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
