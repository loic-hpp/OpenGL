#include "camera.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3 &position, const glm::vec2 &orientation) : m_position(position), m_orientation(orientation)
{
	// TODO: Initialisation des attributs de la classe
}

glm::mat4 Camera::getFirstPersonViewMatrix()
{
	// TODO: Calcul et retour de la matrice de vue en première personne
	// à partir des attributs de la classe
	//
	// En gros à chaque fois qu'on va appuyer sur une touche ou sur la souris,
	// On va modifier la matrice de vue en se servant de la matrice de vue actuelle:
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, m_orientation.x, glm::vec3(0.0f, 1.0f, 0.0f)); // rotation sur l'axe vertical
	rotationMatrix = glm::rotate(rotationMatrix, m_orientation.y, glm::vec3(1.0f, 0.0f, 0.0f)); // rotation sur l'axe horizontal

	// Matrice de translation en fonction de la position
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_position + glm::vec3(0.0f, -1.0f, 0.0f)); // ajuster position en y

	// Matrice de vue combinant rotation et translation
	glm::mat4 viewMatrix = rotationMatrix * translationMatrix;

	return viewMatrix;
}

glm::mat4 Camera::getThirdPersonViewMatrix()
{
	// TODO: Calcul et retour de la matrice de vue en troisième personne
	// à partir des attributs de la classe

	// Décalage initial de π/2 radians sur l'orientation
	float thetaX = m_orientation.x + glm::pi<float>() / 2.0f;
	float thetaZ = m_orientation.y + glm::pi<float>() / 2.0f;

	// Création de la sphère de rayon 6
	float radius = 6.0f;
	float x = radius * glm::sin(thetaZ) * glm::cos(thetaX);
	float y = 1.0f; // Hauteur fixe de la caméra
	float z = radius * glm::sin(thetaZ) * glm::sin(thetaX);

	// Position de la caméra
	glm::vec3 cameraPosition = m_position + glm::vec3(x, y, z);

	// Point observé par la caméra (position du personnage)
	glm::vec3 targetPosition = m_position;

	return glm::lookAt(cameraPosition, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
}
