#ifndef _GROUP_WRAPPER
#define _GROUP_WRAPPER

// #include <scene.h>
#include <group.h>
#include <handler.hpp>

template<class T> Handler<Group> GroupWrapper(Handler<T>);

template<class T>
class WrappedObject : public Group {
  Handler<T> obj;
  friend Handler<Group> GroupWrapper<T>(Handler<T>);
  WrappedObject(Handler<T> o) : obj(o) {} 
public:
  inline void init(Scene& s) override { s.add(obj); }
};

template<> inline Handler<Group> GroupWrapper(Handler<Group> g) { return g; }
template<class T> inline Handler<Group> GroupWrapper(Handler<T> o) { return WrappedObject(o); }

#endif