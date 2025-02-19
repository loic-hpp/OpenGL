#include "shapes.h"

BasicShapeArrays::BasicShapeArrays(const GLfloat *data, GLsizeiptr byteSize)
{
    // TODO Partie 1: Générer et bind le vao de la forme.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // TODO Partie 1: Générer et bind le vbo de la forme.
    // Allouer l'espace nécessaire dans le mode approprié
    // et envoyer les données au gpu.
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_STATIC_DRAW);
}

BasicShapeArrays::~BasicShapeArrays()
{
    // TODO Partie 1: Supprimer la mémoire de l'objet.
    // Assurez-vous que les ressources ne soient pas utilisées
    // pendant la suppression.
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

/// stride is the size that takes this attribut in bytes
void BasicShapeArrays::enableAttribute(GLuint index, GLint size, GLsizei stride, GLsizeiptr offset)
{
    // TODO Partie 1: Activer un attribut et l'attacher correctement au state du vao.
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(offset * sizeof(float)));
    glEnableVertexAttribArray(index);
}

void BasicShapeArrays::draw(GLenum mode, GLsizei count)
{
    // TODO Partie 1: Dessiner la forme.
    glBindVertexArray(m_vao);
    glDrawArrays(mode, 0, count);
    glBindVertexArray(0);
}

BasicShapeMultipleArrays::BasicShapeMultipleArrays(const GLfloat *pos, GLsizeiptr posByteSize, const GLfloat *color, GLsizeiptr colorByteSize)
{
    // TODO Partie 1: Générer et bind le vao de la forme.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // TODO Partie 1: Générer et bind les vbos (un pour la position,
    // l'autre pour la couleur) de la forme. Allouer l'espace
    // nécessaire dans le mode approprié et envoyer les données au gpu.
    glGenBuffers(1, &m_posVbo);
    glGenBuffers(1, &m_colorVbo);

    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, posByteSize, pos, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
    glBufferData(GL_ARRAY_BUFFER, colorByteSize, color, GL_DYNAMIC_DRAW);
}

BasicShapeMultipleArrays::~BasicShapeMultipleArrays()
{
    // TODO Partie 1: Supprimer la mémoire de l'objet.
    // Assurez-vous que les ressources ne soient pas utilisées
    // pendant la suppression.
    glDeleteBuffers(1, &m_posVbo);
    glDeleteBuffers(1, &m_colorVbo);
    glDeleteVertexArrays(1, &m_vao);
}

void BasicShapeMultipleArrays::enablePosAttribute(GLuint index, GLint size, GLsizei stride, GLsizeiptr offset)
{
    // TODO Partie 1: Activer l'attribut de position et l'attacher correctement au state du vao.
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(offset * sizeof(float)));
    glEnableVertexAttribArray(index);
}

void BasicShapeMultipleArrays::enableColorAttribute(GLuint index, GLint size, GLsizei stride, GLsizeiptr offset)
{
    // TODO Partie 1: Activer l'attribut de couleur et l'attacher correctement au state du vao.
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(offset * sizeof(float)));
    glEnableVertexAttribArray(index);
}

void BasicShapeMultipleArrays::updateColorData(const GLfloat *color, GLsizeiptr colorByteSize)
{
    // TODO Partie 1: Modifier la totalité des données de couleur
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, colorByteSize, color);
}

GLfloat *BasicShapeMultipleArrays::mapPosData()
{
    // TODO Partie 1: Activer le mapping des données de position
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    return (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void BasicShapeMultipleArrays::unmapPosData()
{
    // TODO Partie 1: Désactiver le mapping des données de position
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void BasicShapeMultipleArrays::draw(GLenum mode, GLsizei count)
{
    // TODO Partie 1: Dessiner la forme.
    glBindVertexArray(m_vao);
    glDrawArrays(mode, 0, count);
    glBindVertexArray(0);
}

BasicShapeElements::BasicShapeElements()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
}

BasicShapeElements::BasicShapeElements(const GLfloat *data, GLsizeiptr byteSize, const GLuint *indexes, GLsizeiptr indexesByteSize)
{
    // TODO Partie 1: Générer et bind le vao de la forme.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // TODO Partie 1: Générer et bind le vbo et ebo de la forme.
    // Allouer l'espace nécessaire dans le mode approprié
    // et envoyer les données au gpu.
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesByteSize, indexes, GL_STATIC_DRAW);
}

void BasicShapeElements::setData(const GLfloat *data, GLsizeiptr byteSize, const GLuint *indexes, GLsizeiptr indexesByteSize)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesByteSize, indexes, GL_STATIC_DRAW);
}

BasicShapeElements::~BasicShapeElements()
{
    // TODO Partie 1: Supprimer la mémoire de l'objet.
    // Assurez-vous que les ressources ne soient pas utilisées
    // pendant la suppression.
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void BasicShapeElements::enableAttribute(GLuint index, GLint size, GLsizei stride, GLsizeiptr offset)
{
    // TODO Partie 1: Activer un attribut et l'attacher correctement au state du vao.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)(offset * sizeof(float)));
    glEnableVertexAttribArray(index);
}

void BasicShapeElements::draw(GLenum mode, GLsizei count)
{
    // TODO Partie 1: Dessiner la forme avec le ebo.
    glBindVertexArray(m_vao);
    glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
