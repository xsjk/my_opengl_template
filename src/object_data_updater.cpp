#include <object_data_updater.h>
#include <object_data.h>

ObjectDataUpdater::~ObjectDataUpdater() {}

void ObjectDataUpdater::operator()(ObjectData&) const {}

ObjectData ObjectDataUpdater::operator()() const {
    return ObjectData{this};
}