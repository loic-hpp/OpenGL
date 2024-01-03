#include "scenes.h"

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"

#include "utils.h"

TesselationScene::TesselationScene(Resources &resources)
    : Scene(resources), m_viewWireframe(0)
{
    // TODO
    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

TesselationScene::~TesselationScene()
{
}

void TesselationScene::render(glm::mat4 &view, glm::mat4 &projPersp)
{
    drawMenu();

    glm::mat4 mvp;
    glm::mat4 projView = projPersp * view;
    glm::mat4 modelView;

    m_res.tessellation.use();
    m_res.heightmapTexture.use(0);
    m_res.groundTexture.use(1);
    m_res.sandTexture.use(2);
    m_res.snowTexture.use(3);

    mvp = projView;
    glUniformMatrix4fv(m_res.mvpLocationTessellation, 1, GL_FALSE, &mvp[0][0]);
    modelView = view;
    glUniformMatrix4fv(m_res.modelViewLocationTessellation, 1, GL_FALSE, &modelView[0][0]);

    glUniform1i(m_res.viewWireframeLocationTessellation, m_viewWireframe);

    // TODO
    m_res.tesselationPlane.draw(GL_PATCHES, m_res.tesselationPlaneCount);

    mvp = projPersp * glm::mat4(glm::mat3(view));
    drawSky(mvp);
}

void TesselationScene::drawMenu()
{
    ImGui::Begin("Tessellation Parameters");

    ImGui::Checkbox("View wireframe?", (bool *)&m_viewWireframe);

    ImGui::End();
}

static const unsigned int MAX_N_PARTICULES = 10000;
static Particle particles[MAX_N_PARTICULES] = {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0, 0}, {0, 0}, 0}};

ParticleScene::ParticleScene(Resources &resources, Window &w)
    : Scene(resources), m_w(w), m_oldTime(m_w.getTick() / 1000.0f), m_cumulativeTime(0.0f), m_nParticles(1), m_nMaxParticles(1000)
{
    glEnable(GL_PROGRAM_POINT_SIZE);

    // TODO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(2, m_vbo);
    glGenTransformFeedbacks(1, &m_tfo);

    glBindVertexArray(m_vao);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tfo);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_N_PARTICULES, NULL, GL_STREAM_READ);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_N_PARTICULES, particles, GL_STREAM_DRAW);

    // activer les 5 attributs

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
}

ParticleScene::~ParticleScene()
{
    // TODO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_vbo[0]);
    glDeleteBuffers(1, &m_vbo[1]);
    glDeleteBuffers(1, &m_tfo);
    glDeleteVertexArrays(1, &m_vao);
}

void ParticleScene::render(glm::mat4 &view, glm::mat4 &projPersp)
{
    glm::mat4 mvp;
    glm::mat4 projView = projPersp * view;
    glm::mat4 modelView;

    float time = m_w.getTick() / 1000.0;
    float dt = time - m_oldTime;
    m_oldTime = time;
    m_cumulativeTime += dt;
    if (dt > 1.0f)
        m_nParticles = 1;

    m_res.transformFeedback.use();

    // TODO: buffer binding

    glBindVertexArray(m_vao);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tfo);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[1]); // mettre là pour la rétroaction

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, velocity));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, color));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, size));
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, timeToLive));

    glUniform1f(m_res.timeLocationTransformFeedback, time);
    glUniform1f(m_res.dtLocationTransformFeedback, dt);

    // TODO: update particles

    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_POINTS); // LIGNE QUI FAIT PB
    glDrawArrays(GL_POINTS, 0, m_nParticles);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    glBindVertexArray(0);

    // // TODO: swap buffers
    std::swap(m_vbo[0], m_vbo[1]);

    // Draw skybox first, without the function to change some parameter on the depth test.
    glDepthFunc(GL_LEQUAL);
    glDepthMask(0x00);
    m_res.skyboxShader.use();
    m_res.skyboxTexture.use();
    mvp = projPersp * glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(m_res.mvpLocationSky, 1, GL_FALSE, &mvp[0][0]);
    m_res.skybox.draw(GL_TRIANGLES, 6 * 6);
    glDepthMask(0xffffff);
    glDepthFunc(GL_LESS);

    m_res.particule.use();
    m_res.flameTexture.use();

    // TODO: buffer binding

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, velocity));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, color));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, size));
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, timeToLive));

    modelView = view;
    glUniformMatrix4fv(m_res.modelViewLocationParticle, 1, GL_FALSE, &modelView[0][0]);
    glUniformMatrix4fv(m_res.projectionLocationParticle, 1, GL_FALSE, &projPersp[0][0]);

    // TODO: Draw particles without depth write and with blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // changer destination voir énoncé
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_POINTS, 0, m_nParticles);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    if (m_cumulativeTime > 1.0f / 60.0f)
    {
        m_cumulativeTime = 0.0f;
        if (++m_nParticles > m_nMaxParticles)
            m_nParticles = m_nMaxParticles;
    }
}