#ifndef _RENDER_DATA_UPDATER_H_
#define _RENDER_DATA_UPDATER_H_

#include <defines.h>

class ObjectData;

/// @brief an updater directly modifies vertices and indices
class ObjectDataUpdater {
public:
  enum class UpdateType { STATIC, DYNAMIC, STREAM } update_type = UpdateType::DYNAMIC;
  const bool enable_EBO;
  const GLenum draw_mode;
protected:
  mutable bool need_update = true;
public:
  ObjectDataUpdater(GLenum draw_mode, bool enable_EBO);
  virtual ~ObjectDataUpdater();
  bool operator()(ObjectData&) const;
  virtual void update(ObjectData&) const;
  operator ObjectData() const;
};


#include <object_params.h>


class ObjectDataUpdaterPlus : public ObjectParams, public ObjectDataUpdater {
  inline void __update__() { need_update = true; }
  using __base__ = ObjectParams;    // used by DERIVED_SETTER
public:
  using ObjectDataUpdater::ObjectDataUpdater;
  using ObjectDataUpdater::operator();
  void set_params(ObjectData&) const;
  bool operator()(ObjectData&) const;
  ObjectData operator()() const;
  DERIVED_SETTER(buffer_mode)
  DERIVED_SETTER(color)
  DERIVED_SETTER(displacement);
  DERIVED_SETTER(rotation)
  DERIVED_SETTER(scale)
  DERIVED_SETTER(visibility);
};



#endif