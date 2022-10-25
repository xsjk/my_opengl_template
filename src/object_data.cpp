#include <object_data.h>
#include <object_data_updater.h>

std::map<unsigned, ObjectData*> ObjectData::idToObjectData;

ObjectData::ObjectData(const RenderData& handle, vec3 color, Affine affine) : 
  ObjectParams(color, affine), data(handle) {
    __init__();
  }

void ObjectData::draw() const {
  if(visibility)
    data.draw();
}

void ObjectData::update() {
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
        default:
          std::cerr << "known mode" << std::endl;
      }
    }

    // new Version
    switch (buffer_mode) {
      case GL_STATIC_DRAW:
        break;
      case GL_DYNAMIC_DRAW:
        if (need_update) {
          data.update(buffer_mode);
          need_update = false;
        }
        break;
      case GL_STREAM_DRAW:
        data.update(buffer_mode);
        break;
      default:
        std::cerr << "known mode" << std::endl;
    }

}

void ObjectData::onclick(int button, int mode, int pointID) {
  // std::cout << this << " is clicked\n" ;
}

void ObjectData::onrelease(int button, int mode, int pointID) {
  // std::cout << this << " is released\n" ;
}

void ObjectData::ondrag(unsigned x, unsigned y, int pointID) {

}

void ObjectData::mouseover(int pointID) {
  // std::cout << this << " mouse over\n";
}

void ObjectData::mouseout(int pointID) {
  // std::cout << this << " mouse out\n";
}

void ObjectData::mousemove(unsigned x, unsigned y, int pointID) {
  // std::cout << this << " mouse move\n";
}


ObjectData::ObjectData(ObjectData&& other)
  : ObjectParams(other), 
    data(std::move(other.data)),
    updater(std::move(other.updater))
{
  __init__();
}

ObjectData::ObjectData(const ObjectData& other)
  : ObjectParams(other), 
    data(other.data),
    updater(other.updater)
{
  __init__();
}

ObjectData::~ObjectData() {
  idToObjectData.erase(ID);
}

void ObjectData::__init__() {
  idToObjectData.insert({ID,this});
}