#ifndef _ARROW_H_
#define _ARROW_H_

#include <render_data.h>
#include <object_data.h>
#include <group.h>
#include <handler.hpp>

class Scene;

class ArrowMesh : public RenderData {

  public:
    ArrowMesh(float head_radius = 0.1, float body_radius = 0.02, float head_height = 0.2, float body_length = 1, int n = 10);

};

class SphereMesh : public RenderData {

  public:
    SphereMesh(float radius = 1, int n = 10);
};


class Arrow : public Group {  

  protected:

    void __update__();
  
  public:

    vec3 start, end;
    vec3 last_drag;
    vec3 color;
    bool visibility = true;

    bool dragging = false;

    struct ArrowData : public ObjectData {
      Arrow& parent;
      ArrowData(Arrow& arrow);
      virtual void ondrag(unsigned x, unsigned y, vec2 uv) override;
      virtual void mousedown(int button, int mode, vec2 uv) override;
      virtual void mouseup(int button, int mode, vec2 uv) override;
      vec3 get_pos(vec2 uv) const;
    };
    

    Handler<ArrowData> mesh_object{*this};

    SETTER_GETTER(start);
    SETTER_GETTER(end);
    SETTER_GETTER(color);
    SETTER_GETTER(visibility);

    

    Arrow(vec3 start=vec3(0), vec3 end=vec3(0,0,1));

    const static std::function<vec3(float, float)> mesh_generator;
    const static RenderData mesh;


    virtual void init(Scene& scene) override ;


};



class Sphere : public ObjectData {

  const static SphereMesh mesh;

  vec3 center;
  float radius;
  
  public:
    Sphere(vec3 center, float radius);
};



class AxesObject : public Group {

  void __update__();

  // struct Axis : public Arrow {
  //   AxesObject* parent;
  //   Axis(AxesObject* parent, vec3 dir);
  //   virtual void ondrag(unsigned,unsigned,int) override;
  // };
  
  public:
    // AxesObject();

    virtual void init(Scene&) override;

    // virtual void Scene
};


#endif