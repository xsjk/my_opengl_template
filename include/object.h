#ifndef _OBJECT_H_
#define _OBJECT_H_





// this object is high level object

class Scene;


class ObjectBase {

  friend class Scene;

protected:
  Scene& scene;

public:

  ObjectBase(Scene& scene);

  virtual void update() const = 0;

  virtual void draw() const = 0;

  virtual ~ObjectBase();

  

  
};


#include <object_data_updater.h>

// contain updaters and shader
// class Object {
//   public:
//     std::list<ObjectDataUpdaterPlus> updaters;

// };






#endif 