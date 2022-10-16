#include <object_data.h>
#include <object_data_updater.h>
#include <shader.h>

ObjectData::ObjectData() {}

ObjectData::ObjectData(const RenderDataHandler& handle, vec3 color, Affine affine) : 
  Affine(affine), color(color) , data(std::make_shared<std::vector<RenderDataHandler>>()) { data->push_back(handle); }

ObjectData::ObjectData(const std::vector<RenderDataHandler>& handle, vec3 color, Affine affine) : 
  Affine(affine), color(color), data(std::make_shared<std::vector<RenderDataHandler>>(handle)) {}

void ObjectData::draw() const {
  for (auto& handle : *data)
    handle.draw();
}

ObjectData::ObjectData(const ObjectDataUpdater& u)
  : updater(std::make_shared<ObjectDataUpdater>(u)) {}

ObjectData::ObjectData(const ObjectDataUpdater* u) {
  (*u)(*this);
}

void ObjectData::update() {}

void ObjectData::__update__() {}
