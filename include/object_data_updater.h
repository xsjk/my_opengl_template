#ifndef _OBJECT_DATA_UPDATER_H_
#define _OBJECT_DATA_UPDATER_H_

#include <defines.h>

class Vertex;
class RenderDataHandler;
class ObjectData;


/// @brief an updater which directly modifies all attribute of ObjectData
struct ObjectDataUpdater {
  virtual ~ObjectDataUpdater();
  virtual void operator()(ObjectData&) const;
  ObjectData operator()() const;
};


#endif