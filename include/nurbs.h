#ifndef _NURBS_H_
#define _NURBS_H_

#include <b-spline.h>


class NURBSCurve: public BsplineCurve<vec4> {
public:
  using BsplineCurve<vec4>::BsplineCurve;
  
  void set_control_point(int i, vec3 point);
  void set_weight(int i, GLfloat w);

  Vertex evaluate(GLfloat t) const; 

  virtual void update(ObjectData &data) const override;

  double cur_measure(double t) const;

};




#endif