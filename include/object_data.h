#ifndef _OBJECT_DATA_H_
#define _OBJECT_DATA_H_

#include <defines.h>
#include <render_data.h>


#include <object_params.h>
#include <object_data_updater.h>

class Window;
// low level object
class ObjectDataUpdater;
class ObjectDataUpdaterPlus;
class ObjectData : public ObjectParams {

protected:
  friend class Window;
  RenderDataHandler data;

private:
  friend class ObjectDataUpdater;
  friend class ObjectDataUpdaterPlus;
  /// @brief create an object with updater only once (static data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  ObjectData(const T* u) { (*u).update(*this); }

public:

  std::shared_ptr<ObjectDataUpdater> updater = nullptr;

  // this is an old API
  ObjectData(const RenderDataHandler& data, vec3 color=vec3{0}, Affine affine=Affine{});


  /// @brief create an object binding to an updater (dynamic data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  ObjectData(T& u) : ObjectData(std::make_shared<T>(u)) {}

  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  ObjectData(std::shared_ptr<T> u) : data(*u), updater(u) {

    switch (u->update_type) { 
      case ObjectDataUpdater::UpdateType::STATIC: 
        set_buffer_mode(GL_STATIC_DRAW);  break;
      case ObjectDataUpdater::UpdateType::DYNAMIC: 
        set_buffer_mode(GL_DYNAMIC_DRAW); break;
      case ObjectDataUpdater::UpdateType::STREAM: 
        set_buffer_mode(GL_STREAM_DRAW);  break;
    }
    // (*u)(*this);
    u->update(*this); 
  }


  std::vector<Vertex>& vertices = data.vertices;
  std::vector<GLuint>& indices = data.indices;

  void draw() const;
};





#endif