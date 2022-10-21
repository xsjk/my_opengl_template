#include <render_data.h>
#include <object_data.h>
#include <object_data_updater.h>

ObjectDataUpdater::ObjectDataUpdater(GLenum draw_mode, bool enable_EBO) : draw_mode{draw_mode}, enable_EBO{enable_EBO} {}

ObjectDataUpdater::~ObjectDataUpdater() {}

bool ObjectDataUpdater::operator()(ObjectData& data) const {
  switch(update_type) {
    case UpdateType::STATIC:
      break;
    case UpdateType::DYNAMIC:
      if (need_update) {
          update(data);
          need_update = false;
          return true;
      }
      break;
    case UpdateType::STREAM:
      update(data);
      return true;
  }
  return false;
}

void ObjectDataUpdater::update(ObjectData&) const {
  std::cout << "default updater\n";
}


void ObjectDataUpdaterPlus::set_params(ObjectData& data) const {
  data.set_buffer_mode(buffer_mode);
  data.set_color(color);
  data.set_displacement(displacement);
  data.set_rotation(rotation);
  data.set_scale(scale);
  data.set_visibility(visibility);
}

bool ObjectDataUpdaterPlus::operator()(ObjectData& data) const {
  switch(update_type) {
    case UpdateType::STATIC:
      break;
    case UpdateType::DYNAMIC:
      if (need_update) {
          std::cout << "need_update DYNAMIC\n";
          set_params(data);
          update(data);
          need_update = false;
          return true;
      }
      break;
    case UpdateType::STREAM:
      set_params(data);
      update(data);
      return true;
  }
  return false;
}

ObjectData ObjectDataUpdaterPlus::operator()() const { 
  return ObjectData{this};   
}
