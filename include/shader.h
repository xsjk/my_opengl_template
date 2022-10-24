#ifndef _SHADER_H_
#define _SHADER_H_
#include <defines.h>


class ObjectData;
class Camera;
class Light;

class Shader {
  public:
    Shader() = default;
    /**
     * init shader with shader files
     */
    Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath = "");
    void init(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath = "");
    /**
     * init shader with shader codes
     */
    void initWithCode(const std::string &vs, const std::string &fs, const std::string &gs = "");
    /**
     * get shader code from a file
     */
    static std::string getCodeFromFile(const std::string &path);
    /**
     * use shader
     */
    void use() const;
    /**
     * get a uniform variable's location according to its name
     */
    GLint getUniformLocation(const std::string &name) const;
    GLint getAttribLocation(const std::string &name) const;
    /**
     * set value of uniform variables
     */
    void setInt(const std::string &name, GLint value) const;
    void setFloat(const std::string &name, GLfloat value) const;
    void setMat3(const std::string &name, const mat3 &value) const;
    void setMat4(const std::string &name, const mat4 &value) const;
    void setVec3(const std::string &name, const vec3 &value) const;
    void setVec4(const std::string &name, const vec4 &value) const;

    operator GLuint() const;
    bool operator==(const Shader&) const;
    
  private:
    GLuint id = 0;
    static void checkCompileErrors(GLuint shader, const std::string& type);  
};




class ObjectShader : public Shader {
  public:
    using Shader::Shader;
    void set(const Camera &) const;
    void set(const Light &, GLuint) const;
    void draw(const ObjectData &) const; 
    void draw(const ObjectData &, float) const;   
};



/// @brief this shader is only used for uploading the ID of each object to the GPU
class PickingShader : public Shader {
  static const char* vs_file, *fs_file;
  public:
    using Shader::Shader;
    void set(const Camera &) const;
    void init();
    void draw(const ObjectData &) const;
};


class LightShader : public Shader {
  public:
    using Shader::Shader;
    void set(const Camera &) const;
    void draw(const Light &) const;
};

#endif