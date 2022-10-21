#include <object_data.h>
#include <object_data_updater.h>

ObjectData::ObjectData(const RenderDataHandler& handle, vec3 color, Affine affine) : 
  ObjectParams(color, affine), data(handle) {}

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
