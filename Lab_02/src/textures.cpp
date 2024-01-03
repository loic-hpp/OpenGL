#include "textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

Texture2D::Texture2D(const char *path, GLenum wrapMode)
{
    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(true);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned char *data = stbi_load(path, &width, &height, &nChannels, 0);
    if (data == NULL)
    {
        std::cout << "Error loading texture \"" << path << "\": " << stbi_failure_reason() << std::endl;
        m_id = 0; // Initialisation de l'ID de texture à zéro pour indiquer une texture invalide
        return;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glActiveTexture(GL_TEXTURE0);

    GLenum format;
    if (nChannels == 3)
    {
        format = GL_RGB;
    }
    else if (nChannels == 4)
    {
        format = GL_RGBA;
    }
    else
    {
        std::cout << "Unsupported image format: " << nChannels << " channels" << std::endl;
        // Gérer l'erreur ou l'incompatibilité du format d'image ici
        stbi_image_free(data);
        return;
    }

    // Maintenant, vous savez quel format utiliser pour glTexImage2D
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode); // Mode de répétition horizontal
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode); // Mode de répétition vertical

    // Libérer la mémoire des données de l'image
    stbi_image_free(data);
}

Texture2D::~Texture2D()
{
    // TODO: Supprimer la mémoire de l'objet
    glGenBuffers(1, &m_id);
}

void Texture2D::enableMipmap()
{
    // TODO: Activer le mipmap. N'oublier pas de modifier les paramètres de texture.
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::use()
{
    // TODO: Utilise la texture
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unuse()
{
    // TODO: N'utilise aucune texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureCubeMap::TextureCubeMap(const char **pathes)
{
    const size_t N_TEXTURES = 6;
    unsigned char *datas[N_TEXTURES];
    int widths[N_TEXTURES];
    int heights[N_TEXTURES];
    int nChannels[N_TEXTURES];
    stbi_set_flip_vertically_on_load(false);
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    for (unsigned int i = 0; i < 6; i++)
    {
        datas[i] = stbi_load(pathes[i], &widths[i], &heights[i], &nChannels[i], 0);
        if (datas[i] == NULL)
            std::cout << "Error loading texture \"" << pathes[i] << "\": " << stbi_failure_reason() << std::endl;
        if (datas[i])
        {
            GLenum format;
            if (nChannels[i] == 1)
                format = GL_RED;
            else if (nChannels[i] == 3)
                format = GL_RGB;
            else if (nChannels[i] == 4)
                format = GL_RGBA;
            else
            {
                std::cout << "Unsupported image format: " << nChannels[i] << " channels" << std::endl;
                // Gérer l'erreur ou l'incompatibilité du format d'image ici
                stbi_image_free(datas[i]);
                return;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, widths[i], heights[i], 0, format, GL_UNSIGNED_BYTE, datas[i]);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Libérer la mémoire des données de l'image
    for (unsigned int i = 0; i < 6; i++)
    {
        stbi_image_free(datas[i]);
    }
}

TextureCubeMap::~TextureCubeMap()
{
    // TODO: Supprimer la mémoire de l'objet
    glDeleteTextures(1, &m_id);
}

void TextureCubeMap::use()
{
    // TODO: Utilise la texture du cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}
