#ifndef _OBJECT_DATA_H_
#define _OBJECT_DATA_H_

#include <utils.h>
#include <render_data.h>


#include <object_params.h>
#include <object_data_updater.h>

class Window;
class Picking;
// low level object
class ObjectDataUpdater;
class ObjectDataUpdaterPlus;
class ObjectData : public ObjectParams, public Indexer {

  friend class Window;
  friend class Picking;

  // bool selected = false;

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

  virtual void onclick(int button, int mods, int pointID=-1);
  virtual void onrelease(int button, int mods, int pointID=-1);
  virtual void ondrag(unsigned x, unsigned y, int pointID=-1);
  virtual void mouseover(int pointID=-1);
  virtual void mouseout(int pointID=-1);
  virtual void mousemove(unsigned x, unsigned y, int pointID=-1);

  virtual void MouseCallback(int button, int action, int mods) {};

  inline std::vector<Vertex>& vertices() const { return data.vertices(); }
  inline std::vector<GLuint>& indices() const { return data.indices(); }
  
  void draw() const;
  virtual void update();

  SETTER_GETTER(need_update, bool)
};


// class ObjectDataHandler {
//   std::shared_ptr<ObjectData> data;
// public:
//   template<typename ...T>
//   ObjectDataHandler(T&&... args) : data(std::make_shared<ObjectData>(std::forward<T>(args)...)) {}

//   ObjectDataHandler(std::shared_ptr<ObjectData> data) : data(data) {}

//   inline ObjectData& operator*() { return *data; }
//   inline ObjectData* operator->() { return data.get(); }

//   inline bool operator==(const ObjectDataHandler& other) const { return data == other.data; }

//   inline unsigned get_ID() const { return data->get_ID(); }
// };


using Object = Handler<ObjectData>;



#endif