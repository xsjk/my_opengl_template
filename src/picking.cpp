
#include <picking.h>

PickingTexture::~PickingTexture() {
  if (m_fbo != 0) {
    glDeleteFramebuffers(1, &m_fbo);
  }
  if (m_pickingTexture != 0) {
    glDeleteTextures(1, &m_pickingTexture);
  }
  if (m_depthTexture != 0) {
    glDeleteTextures(1, &m_depthTexture);
  }
}


bool PickingTexture::init(unsigned int WindowWidth, unsigned int WindowHeight) {
  // Create the FBO
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  // Create the texture object for the primitive information buffer
  glGenTextures(1, &m_pickingTexture);
  glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, WindowWidth, WindowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);

  // Create the texture object for the depth buffer
  glGenTextures(1, &m_depthTexture);
  glBindTexture(GL_TEXTURE_2D, m_depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

  // Verify that the FBO is correct
  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (Status != GL_FRAMEBUFFER_COMPLETE) {
    printf("FB error, status: 0x%x\n", Status);
    exit(1);
  }

  // Restore the default framebuffer
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}


void PickingTexture::activate() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void PickingTexture::deactivate() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}



vec3 PickingTexture::read(unsigned int x, unsigned int y) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

  glReadBuffer(GL_COLOR_ATTACHMENT0);

  vec3 Pixel;
  glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

  glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  return Pixel;
}

#include <object_data.h>
#include <camera.h>


void PickingShader::set(const Camera& camera) const {
  setMat4("camera", camera.matrix());
}


void PickingShader::init() {
  Shader::init("shader/picking.vs", "shader/picking.fs");
}

void PickingShader::draw(const ObjectData& object) const {
  setMat4("model", object.matrix());
  setVec3("ID", {float(object.get_ID()),0.0,0.0});
  object.draw();
}


void Picking::init(unsigned width, unsigned height) {
  texture.init(width, height);
  shader.init();
}

#include <scene.h>

void Picking::update(const std::vector<Handler<Scene>>& scenes) {
  texture.activate();
  for(auto& scene: scenes)
    scene->render(&shader);
  texture.deactivate();
}

vec3 Picking::get_current(unsigned x, unsigned y) {
  auto res = texture.read(x,y);
  current = round(res.r);
  pointID = round(res.g);
  if(current > 0) {
    try {
      current_obj = ObjectData::idToObjectData.at(current);
    } catch (std::out_of_range& e) {
      current_obj = nullptr;
    }
  } else {
    current_obj = nullptr;
  }
  return res;
}

vec3 Picking::get_current(glm::vec<2, unsigned> pos) {
  return get_current(pos.x, pos.y);
}