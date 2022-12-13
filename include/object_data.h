#ifndef _OBJECT_DATA_H_
#define _OBJECT_DATA_H_

#include <utils.h>
#include <render_data.h>


#include <object_params.h>
#include <object_data_updater.h>

class Window;
class Picking;
class Group;
class Scene;
// low level object
class ObjectDataUpdater;
class ObjectDataUpdaterPlus;



class AbsoluteObjectData : public AbsoluteObjectParams, public Indexer {
  friend class Window;
  friend class Picking;
  friend class Group;
  friend class Scene;

protected:
  static std::map<unsigned, AbsoluteObjectData*> idToObjectData;
  RenderData data;
  bool need_update = false; // use of dynamic draw


private:
  friend class ObjectDataUpdater;
  friend class ObjectDataUpdaterPlus;
  /// @brief create an object with updater only once (static data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  AbsoluteObjectData(const T* u) { 
    __init__();
    (*u).update(*this); 
  }

  void __init__();

public:
  Group* parent = nullptr;
  Scene* scene = nullptr;


  AbsoluteObjectData(AbsoluteObjectData&&);
  AbsoluteObjectData(const AbsoluteObjectData&);

  virtual ~AbsoluteObjectData();

  std::shared_ptr<ObjectDataUpdater> updater = nullptr;

  // this is an old API
  AbsoluteObjectData(const RenderData& data, vec3 color=vec3{0}, Affine affine=Affine{});

  /// @brief create an object binding to an updater (dynamic data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  AbsoluteObjectData(T& u) : AbsoluteObjectData(std::make_shared<T>(u)) { __init__(); }

  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  AbsoluteObjectData(std::shared_ptr<T> u) : data(*u), updater(u) {
    __init__();

    switch (u->update_type) { 
      case ObjectDataUpdater::UpdateType::STATIC: 
        set_buffer_mode(GL_STATIC_DRAW);  break;
      case ObjectDataUpdater::UpdateType::DYNAMIC: 
        set_buffer_mode(GL_DYNAMIC_DRAW); break;
      case ObjectDataUpdater::UpdateType::STREAM: 
        set_buffer_mode(GL_STREAM_DRAW);  break;
    }
    u->update(*this); 
  }

  inline unsigned get_ID() const { return ID; }

  virtual void mousedown(int button, int mods, int pointID);
  virtual void mousedown(int button, int mods, vec2 uv);
  virtual void mouseup(int button, int mods, int pointID);
  virtual void mouseup(int button, int mods, vec2 uv);
  virtual void ondrag(unsigned x, unsigned y, int pointID);
  virtual void ondrag(unsigned x, unsigned y, vec2 uv);
  virtual void mouseover(int pointID);
  virtual void mouseover(vec2 uv);
  virtual void mouseout(int pointID);
  virtual void mouseout(vec2 uv);
  virtual void mousemove(unsigned x, unsigned y, vec2 uv);
  virtual void mousemove(unsigned x, unsigned y, int pointID);
  
  virtual void MouseCallback(int button, int action, int mods) {};

  inline auto& vertices() const { return data.vertices(); }
  inline auto& indices() const { return data.indices(); }

  void draw() const;
  virtual void update();

  SETTER_GETTER(need_update)
  GETTER(data)
};



class ObjectData : public ObjectParams, public Indexer {

  friend class Window;
  friend class Picking;
  friend class Group;
  friend class Scene;


protected:
  static std::map<unsigned, ObjectData*> idToObjectData;
  RenderData data;
  bool need_update = false; // use of dynamic draw


private:
  friend class ObjectDataUpdater;
  friend class ObjectDataUpdaterPlus;
  /// @brief create an object with updater only once (static data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  ObjectData(const T* u) { 
    __init__();
    (*u).update(*this); 
  }

  void __init__();

public:
  Group* parent = nullptr;
  Scene* scene = nullptr;


  ObjectData(ObjectData&&);
  ObjectData(const ObjectData&);

  virtual ~ObjectData();

  std::shared_ptr<ObjectDataUpdater> updater = nullptr;

  // this is an old API
  ObjectData(const RenderData& data, vec3 color=vec3{0}, Affine affine=Affine{});

  /// @brief create an object binding to an updater (dynamic data)
  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  ObjectData(T& u) : ObjectData(std::make_shared<T>(u)) { __init__(); }

  template<class T, typename U=std::enable_if_t<std::is_base_of_v<ObjectDataUpdater,T>>>
  ObjectData(std::shared_ptr<T> u) : data(*u), updater(u) {
    __init__();

    switch (u->update_type) { 
      case ObjectDataUpdater::UpdateType::STATIC: 
        set_buffer_mode(GL_STATIC_DRAW);  break;
      case ObjectDataUpdater::UpdateType::DYNAMIC: 
        set_buffer_mode(GL_DYNAMIC_DRAW); break;
      case ObjectDataUpdater::UpdateType::STREAM: 
        set_buffer_mode(GL_STREAM_DRAW);  break;
    }
    u->update(*this); 
  }

  inline unsigned get_ID() const { return ID; }

  virtual void mousedown(int button, int mods, int pointID);
  virtual void mousedown(int button, int mods, vec2 uv);
  virtual void mouseup(int button, int mods, int pointID);
  virtual void mouseup(int button, int mods, vec2 uv);
  virtual void ondrag(unsigned x, unsigned y, int pointID);
  virtual void ondrag(unsigned x, unsigned y, vec2 uv);
  virtual void mouseover(int pointID);
  virtual void mouseover(vec2 uv);
  virtual void mouseout(int pointID);
  virtual void mouseout(vec2 uv);
  virtual void mousemove(unsigned x, unsigned y, vec2 uv);
  virtual void mousemove(unsigned x, unsigned y, int pointID);

  virtual void MouseCallback(int button, int action, int mods) {};

  inline auto& vertices() const { return data.vertices(); }
  inline auto& indices() const { return data.indices(); }
  
  void draw(bool force_draw=false) const;
  virtual void update();

  SETTER_GETTER(need_update)
  GETTER(data)
};



#endif