#ifndef _GROUP_H_
#define _GROUP_H_


class Scene;

struct Group {

  Scene* scene = nullptr;

  Group* parent = nullptr;


  virtual void init(Scene &scene) = 0;

  virtual void update() {};

  virtual ~Group() = default;

  virtual void KeyboardCallback(int key, int scancode, int action, int mods) {}


};




#endif