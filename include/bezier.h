#ifndef _BEZIER_H_
#define _BEZIER_H_
#include <defines.h>
#include <vertex.h>
#include <object_data_updater.h>




class BezierCurve: public ObjectDataUpdaterPlus {

 public:

  Triangulation mode = Triangulation::adaptive;

  std::vector<vec3> control_points;

  GLuint resolution = 10;


  BezierCurve(int m);
  BezierCurve(const std::vector<vec3>& control_points);

  void set_control_point(int i, vec3 point);
  
  Vertex evaluate(GLfloat t) const;
  
  /// @brief the measure of bending at t (not equal to curvature!)
  double cur_measure(double t) const;

  virtual void update(ObjectData&) const override;
  
  /// @brief calculate the coefficents of the n  Bernstein basis polynomial's k-th derivative
  /// @param p the order of the Bernstein polynomial
  /// @param t the parametre
  /// @param k the order of the derivative
  /// @return the coefficient vector B_{i,p}(t) (for i)
  static const std::vector<GLfloat>& coeffs(GLushort p, GLfloat t, GLushort k=0);

  /// @brief the Cache for coeffs， map from (p,k,t) to the coefficient vector
  /// @note the cache is shared by all BezierCurve objects
  static struct Cache {
    struct Key {
      GLushort p, k; 
      GLfloat t; 
      inline operator size_t() const { 
        return *reinterpret_cast<const size_t*>(this); 
      }
    };
    inline std::vector<GLfloat>& operator[](Key k) { return data[k]; }
  private:
    std::unordered_map<Key, std::vector<GLfloat>, std::hash<size_t>> data;
  } cache;

  static Vertex evaluate(const std::vector<vec3>& control_points, GLfloat t);

};


#include <sampler.hpp>

class BezierSurface : public ObjectDataUpdaterPlus {

public:

  Triangulation mode = Triangulation::adaptive;


  std::vector<std::vector<vec3>> control_points;

  BezierSurface(int m, int n);
  BezierSurface(const std::vector<std::vector<vec3>>& control_points);

  virtual Vertex evaluate(GLfloat u, GLfloat v) const;

/// @brief the measure of bending at (u,v) (not equal to curvature!)
  double cur_measure(double u, double v, double order=3) const;

  virtual void update(ObjectData&) const override;

  
  static Vertex evaluate(const std::vector<std::vector<vec3>>& control_points, GLfloat u, GLfloat v);

  static std::vector<BezierSurface> read(const std::string &path);

};


#endif