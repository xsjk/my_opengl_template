#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <defines.h>
#include <transform.h> // Affine
#include <render_data.h>


class Window;
class ObjectDataUpdater;
class ObjectData : public Affine {
  
  friend class Window;
  
  friend class ObjectDataUpdater;
  ObjectData(const ObjectDataUpdater * updater);

protected:
  ObjectData();
  std::shared_ptr<std::vector<RenderDataHandler>> data;
  std::shared_ptr<ObjectDataUpdater> updater = nullptr;

private:
  vec3 color;
  void __update__();
public:
  SETTER_GETTER(color, vec3)

  virtual void update();

  inline bool is_static() const { return updater == nullptr; }


  /// @brief create dynamic object data controlled by the updater
  /// @param updater 
  ObjectData(const ObjectDataUpdater& updater);

  // this should be removed
  ObjectData(const RenderDataHandler& data, vec3 color=vec3{0}, Affine affine=Affine{});
  
  ObjectData(const std::vector<RenderDataHandler>& data, vec3 color=vec3{0}, Affine affine=Affine{});

  std::vector<RenderDataHandler>& render_data = *data;

  void draw() const;
};

class ObjectDataHandler : public ObjectData {};

using Object = ObjectDataHandler;

#endif