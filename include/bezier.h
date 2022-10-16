#ifndef _BEZIER_H_
#define _BEZIER_H_
#include <defines.h>
#include <vertex.h>
#include <render_data_updater.h>


class BezierCurve: public RenderDataUpdater {

 public:
  using RenderDataUpdater::operator();

  std::vector<vec3> control_points_;

  GLuint resolution = 100;


  BezierCurve(int m);
  BezierCurve(const std::vector<vec3>& control_points);

  void setControlPoint(int i, vec3 point);
  
  Vertex evaluate(GLfloat t) const;

  std::vector<GLfloat> coeff(GLfloat t) const;

  virtual void operator()(std::vector<Vertex>&, std::vector<GLuint>&) const override;

  /// @brief calculate C(n, i) * t^i * (1-t)^(n-i) at n,t
  /// @param n the order of the polynomial
  /// @param t the parameter
  /// @return the list of Bernstein coefficents of degree n at t
  static std::vector<GLfloat> coeff(GLuint n, GLfloat t);
  static std::vector<GLfloat> dcoeff(const std::vector<GLfloat>& coeff, GLfloat t);
  
  static Vertex evaluate(const std::vector<vec3>& control_points, GLfloat t);

};



class BezierSurface : public RenderDataUpdater {
 public:
  using RenderDataUpdater::operator();
  
  std::vector<std::vector<vec3>> control_points_m_;
  std::vector<std::vector<vec3>> control_points_n_;

  GLuint resolution_m = 20;
  GLuint resolution_n = 20;

  BezierSurface(int m, int n);
  BezierSurface(const std::vector<std::vector<vec3>>& control_points);

  Vertex evaluate(GLfloat u, GLfloat v) const;

  void setControlPoint(int i, int j, vec3 point);

  virtual void operator()(std::vector<Vertex>&, std::vector<GLuint>&) const override;

  static std::vector<BezierSurface> read(const std::string &path);
  static Vertex evaluate(const std::vector<std::vector<vec3>>& control_points, GLfloat u, GLfloat v);

};

#endif