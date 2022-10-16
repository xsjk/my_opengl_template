#ifndef _BEZIER_OBJECT_H_
#define _BEZIER_OBJECT_H_

#include <defines.h>
#include <object_data_updater.h>
#include <object_data.h>


class BezierSurface;

class BezierSurfaces : public ObjectDataUpdater {

public:
  using ObjectDataUpdater::operator();

  virtual void operator()(ObjectData& data) const override;


  std::vector<BezierSurface> surface_generators; // data


  public:
    /// @brief create bezier surfaces from file
    /// @param path filepath
    BezierSurfaces(const std::string& path);


    BezierSurfaces(const std::vector<BezierSurface>& surfaces);

    /// @brief create bezier surface from a single surface
    /// @param surface 
    BezierSurfaces(const BezierSurface& surface);


    /// @brief get the i-th surface_generator
    /// @param i 
    /// @return BezierSurface
    BezierSurface& operator[](GLuint i);


};



#endif