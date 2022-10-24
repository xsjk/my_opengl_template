#include <object_data.h>
#include <object_data_updater.h>

std::map<unsigned, ObjectData*> ObjectData::idToObjectData;

ObjectData::ObjectData(const RenderDataHandler& handle, vec3 color, Affine affine) : 
  ObjectParams(color, affine), data(handle) {
    __init__();
  }

void ObjectData::draw() const {
  if(updater){
    // new Version
    switch (buffer_mode) {
      case GL_STATIC_DRAW:
        // std::cout << "STATIC\n";
        break;
      case GL_DYNAMIC_DRAW:
        if ((*updater)(const_cast<ObjectData&>(*this)))
          data.update(buffer_mode);
        // std::cout << "DYNAMIC\n";
        break;
      case GL_STREAM_DRAW:
        (*updater)(const_cast<ObjectData&>(*this));
        data.update(buffer_mode);
        // std::cout << "STREAM\n";
        break;
    }
    
  }
  // old Version
  if(visibility)
    data.draw();
}


void ObjectData::onclick() {
  std::cout << this << " is clicked\n" ;
}

void ObjectData::onrelease() {
  std::cout << this << " is released\n" ;
}

void ObjectData::mouseover() {
  std::cout << this << " mouse over\n";
}

void ObjectData::mouseout() {
  std::cout << this << " mouse out\n";
}

ObjectData::~ObjectData() {
  std::cout << "map erase " << ID << std::endl;
  idToObjectData.erase(ID);
}

void ObjectData::__init__() {
  std::cout << "map insert " << ID << std::endl;
  idToObjectData.insert({ID,this});
}