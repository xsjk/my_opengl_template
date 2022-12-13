#include <object_data.h>
#include <object_data_updater.h>

std::map<unsigned, ObjectData*> ObjectData::idToObjectData;

ObjectData::ObjectData(const RenderData& handle, vec3 color, Affine affine) : 
  ObjectParams(color, affine), data(handle) {
    __init__();
  }

void ObjectData::draw(bool force_draw) const {
  if(force_draw || visibility)
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
      std::cerr << "unknown mode" << std::endl;
  }

}

void ObjectData::mousedown(int button, int mode, int pointID) {
  std::cout << this << " is clicked\n" ;
}

void ObjectData::mousedown(int button, int mode, vec2 uv) {
  std::cout << "click" << uv << std::endl;
  if(data.draw_mode == GL_POINTS)
    mousedown(button, mode, round(uv[0]));
}

void ObjectData::mouseup(int button, int mode, int pointID) {
  std::cout << this << " is released\n" ;
}

void ObjectData::mouseup(int button, int mode, vec2 uv) {
  if(data.draw_mode == GL_POINTS)
    mouseup(button, mode, round(uv[0]));
}

void ObjectData::ondrag(unsigned x, unsigned y, int pointID) {
  std::cout << this << " is dragged\n" ;
}

void ObjectData::ondrag(unsigned x, unsigned y, vec2 uv) {
  if(data.draw_mode == GL_POINTS)
    ondrag(x, y, round(uv[0]));
}

void ObjectData::mouseover(int pointID) {
  std::cout << this << " mouse over\n";
}

void ObjectData::mouseover(vec2 uv) {
  if(data.draw_mode == GL_POINTS)
    mouseover(round(uv[0]));
}

void ObjectData::mouseout(int pointID) {
  std::cout << this << " mouse out\n";
}

void ObjectData::mouseout(vec2 uv) {
  if(data.draw_mode == GL_POINTS)
    mouseout(round(uv[0]));
}

void ObjectData::mousemove(unsigned x, unsigned y, int pointID) {
  // std::cout << this << " mouse move\n";
}

void ObjectData::mousemove(unsigned x, unsigned y, vec2 uv) {
  if(data.draw_mode == GL_POINTS)
    mousemove(x, y, round(uv[0]));
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