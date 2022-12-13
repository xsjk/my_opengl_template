#ifndef _WORLD_H
#define _WORLD_H

#include <scene.h>
#include <group_wrapper.hpp>


class World : public Group {

  std::vector<Handler<Group>> world_objects;

public:
  World() = default;

  inline virtual void init(Scene& s) {
    for (auto&& object : world_objects)
      s.add(object);
  }

  virtual void update() {
  };

  inline Handler<Group>& operator[](unsigned i) {
    return world_objects[i];
  }

  inline void add(Handler<Group> object) {
    object->parent = this;
    world_objects.push_back(std::move(object));
  }

  inline void add(Handler<ObjectData> raw_object) {
    world_objects.push_back(GroupWrapper(raw_object));
  }

  void collide() {

  }

};



#endif