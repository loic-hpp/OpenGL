#ifndef UTILS_H
#define UTILS_H

double rand01();

void getGroupRandomPos(int groupId, const int N_ROWS, float &x, float &z);

// Chemin pour les textures de skybox
/*
const char* pathes[] = {
    "skybox/Daylight Box_Right.bmp",
    "skybox/Daylight Box_Left.bmp",
    "skybox/Daylight Box_Top.bmp",
    "skybox/Daylight Box_Bottom.bmp",
    "skybox/Daylight Box_Front.bmp",
    "skybox/Daylight Box_Back.bmp",
};
*/

// Dans l'initialisation dans le main:
/*
    const int N_ROWS = 7;
    const int N_GROUPS = N_ROWS * N_ROWS;

    glm::mat4 groupsTransform[N_GROUPS];

    glm::mat4 treeTransform[N_GROUPS];
    glm::mat4 rockTransform[N_GROUPS];
    glm::mat4 shroomTransform[N_GROUPS];

    // ...

    glm::vec3 playerPosition = glm::vec3(0);
    glm::vec2 playerOrientation = glm::vec2(0);

    // ...

    bool isFirstPersonCam = false;

*/

// Pour la partie 3 avec les positions aléatoires du gazon:
/*
    float randomX = -25.0f + rand01() * 50.0f;
    float randomZ = -25.0f + rand01() * 50.0f;
    double rval = rand01();
    int randomTexture = rval > 0.05f ? rval > 0.10f ? 0 : 1 : 2; // Numero de texture entre [0,2]
*/

// Pour le positionnement/dessin des groupes, utilisez
// for (int i = 0; i < N_GROUPS; ++i)

// Pour les input de souris
// int x=0, y=0;
// w.getMouseMotion(x, y);

// Pour le clavier
// w.getKeyHold(Window::Key::W)

// Votre main loop devrait dessiner dans un ordre semblable:

// Ruisseau   - partie 2
// Gazon      - partie 3
// Personnage (si besoin)
// Sol
// Modèles (autant que possible tous ensembles, tous les arbres, tous les rochés, tous les champignons)
// HUD (carré/coeur dans l'écran)
// Skybox - partie 2

#endif // UTILS_H