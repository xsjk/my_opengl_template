#include <shader.h>
#include <utils.h>

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
  init(vsPath, fsPath, gsPath);
}

void Shader::init(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
  initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), getCodeFromFile(gsPath));
}

void Shader::initWithCode(const std::string &vs, const std::string &fs, const std::string &gs) {
  //set shaders to OpenGL
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char *vstr = vs.c_str();
  glShaderSource(vertexShader, 1, &vstr, NULL);
  glCompileShader(vertexShader);
  checkCompileErrors(vertexShader,"VERTEX");

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fstr = fs.c_str();
  glShaderSource(fragmentShader, 1, &fstr, NULL);
  glCompileShader(fragmentShader);
  checkCompileErrors(fragmentShader,"FRAGMENT");

  GLuint geometryShader = 0;
  if (!gs.empty()) {
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    const char *gstr = gs.c_str();
    glShaderSource(geometryShader, 1, &gstr, NULL);
    glCompileShader(geometryShader);
    checkCompileErrors(geometryShader,"GEOMETRY");
  }
  
  //create shader program
  id = glCreateProgram();
  glAttachShader(id, vertexShader);
  glAttachShader(id, fragmentShader);
  if (!gs.empty()) glAttachShader(id, geometryShader);

  glLinkProgram(id);
  checkCompileErrors(id,"PROGRAM");

  //delete shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  if (!gs.empty()) glDeleteShader(geometryShader);

}



std::string Shader::getCodeFromFile(const std::string &path) {
  if (path.empty()) return "";
  std::string code;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(getPath(path));
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure &e) {
    LOG_ERR("File Error: " + std::string(e.what()));
  }
  return code;
}

void Shader::use() const { glUseProgram(id); }

GLint Shader::getUniformLocation(const std::string &name) const {
  return glGetUniformLocation(id, name.c_str());
}

GLint Shader::getAttribLocation(const std::string &name) const {
  return glGetAttribLocation(id, name.c_str());
}

void Shader::setInt(const std::string &name, GLint value) const {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat3(const std::string &name, const mat3 &value) const {
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const {
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

Shader::operator GLuint() const {
  return id;
}

bool Shader::operator==(const Shader& other) const {
  return id ==other.id;
}


void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM"){
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success){
          glGetShaderInfoLog(shader, 1024, NULL, infoLog);
          std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
  } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
          glGetProgramInfoLog(shader, 1024, NULL, infoLog);
          std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
  }
}


#include <camera.h>
#include <object_data.h>
#include <light.h>
#include <render_data.h>


void ObjectShader::set(const Camera& camera) const {
  setMat4("camera", camera.matrix());
  setVec3("viewPos", camera.get_eye());
}

void ObjectShader::set(const Light& light, GLuint i) const {
  auto name = "light[" + std::to_string(i) + "]";
  setVec4(name + ".position", light.position);
  setFloat(name + ".constant", light.constant);
  setFloat(name + ".linear", light.linear);
  setFloat(name + ".quadratic", light.quadratic);
  setVec3(name + ".direction", light.direction);
  setFloat(name + ".cutOff", light.cutOff);
  setFloat(name + ".outerCutOff", light.outerCutOff);
  setVec3(name + ".color", light.color);
  setVec3(name + ".ambient", light.ambient);
  setVec3(name + ".diffuse", light.diffuse);
  setVec3(name + ".specular", light.specular);
}

void ObjectShader::draw(const ObjectData& object) const {
  setVec3("color", object.get_color());
  setMat4("model", object.matrix());
  setFloat("time", glfwGetTime());  
  object.draw();
}

void ObjectShader::draw(const ObjectData& object, float t) const {
  setFloat("t", t);
  draw(object);
}

void LightShader::set(const Camera& camera) const {
  setMat4("camera", camera.matrix());
}

void LightShader::draw(const Light& light) const {
  if (light.direction==vec3(0.0f)) {
    setMat4("model", translate(mat4{1}, vec3(light.position)));
  } else {
    if (light.direction.x==0.0f && light.direction.z==0.0f) {
      auto model = translate(mat4{1}, vec3(light.position));
      if (light.direction.y<0) model[1] *= -1;
      setMat4("model", model);
    } else {
      vec3 axis = cross(vec3(0, 1, 0), light.direction);
      float angle = acos(dot(normalize(light.direction), vec3(0, 1, 0)));
      setMat4("model", translate(mat4{1}, vec3(light.position)) * rotate(mat4{1}, angle, axis));
    }
  }

  setVec4("light" ".position", light.position);
  setFloat("light" ".constant", light.constant);
  setFloat("light" ".linear", light.linear);
  setFloat("light" ".quadratic", light.quadratic);
  setVec3("light" ".direction", light.direction);
  setFloat("light" ".cutOff", light.cutOff);
  setFloat("light" ".outerCutOff", light.outerCutOff);
  setVec3("light" ".color", light.color);
  setVec3("light" ".ambient", light.ambient);
  setVec3("light" ".diffuse", light.diffuse);
  setVec3("light" ".specular", light.specular);


  setFloat("time", glfwGetTime());

  light.display.draw();
}



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