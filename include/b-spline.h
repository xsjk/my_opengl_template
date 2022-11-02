#ifndef _B_SPLINE_H_
#define _B_SPLINE_H_
#include <vertex.h>
#include <object_data_updater.h>
#include <bezier.h>

class BsplineCurve: public ObjectDataUpdaterPlus {

 public:

  Triangulation mode = Triangulation::uniform;

  /// @brief type of the knot vector
  enum class Type {
    clamped,
    open,
    uniform,
    closed
  } type = Type::clamped;

  std::vector<vec3> control_points;
  GLushort p; // order of the spline

  GLuint resolution = 10;


  BsplineCurve(GLushort m, GLushort p=2, Type type=Type::clamped);
  BsplineCurve(const std::vector<vec3>& control_points, GLushort p=2, Type type = Type::clamped);

  void set_control_point(int i, vec3 point);
  
  Vertex evaluate(GLfloat t) const;

  double cur_measure(double t) const;

  virtual void update(ObjectData&) const override;

  const std::vector<GLfloat>& coeff(GLfloat t) const;

  /// @brief get the coefficient vector B_{i,n}(t) (for i=0...) of given knot vector
  /// @param p the order of the curve
  /// @param t the parametre
  /// @param u the knot vector
  /// @param k the order of derivative (default 0 i.e. the curve itself)
  /// @return the coefficient vector
  static const std::vector<GLfloat>& coeffs(GLushort p, GLfloat t, const std::vector<GLfloat>& u, GLushort k=0);

  /// @brief get the coefficient vector B_{i,n}(t) (for i=0...) of the knot vector of given type
  /// @param p the order of the curve
  /// @param t the parametre
  /// @param type the type of the knot vector
  /// @param k the order of derivative (default 0 i.e. the curve itself)
  static const std::vector<GLfloat>& coeffs(GLushort n, GLushort p, GLfloat t, Type type=Type::clamped, GLushort k=0);

  /// @brief get the knot vector for a given type
  /// @param n the number of control points
  /// @param p the order of the curve
  /// @param type the type of the curve: clamped, open or closed
  /// @return the knot vector
  static std::vector<GLfloat> knot(GLushort n, GLushort p, Type type);

  using Cache = std::unordered_map<std::vector<GLfloat>, BezierCurve::Cache>;
  /// @brief the map from knots vector to the corresponding cache which is a map from (p,k,t) to the coefficient vector
  /// @note the cache is shared among all BsplineCurve objects
  static Cache cache;

  /// @brief evaluate the curve of given order and knot vector
  /// @param control_points the control points
  /// @param p the order of the curve
  /// @param t the parametre
  /// @param type the type of the curve: clamped, open or closed
  /// @return the vertex at t
  static Vertex evaluate(const std::vector<vec3>& control_points, GLushort p, GLfloat t, Type type=Type::clamped);

};



class BsplineSurface : public ObjectDataUpdaterPlus {
  
public:
  using Type = typename BsplineCurve::Type;
  
  glm::vec<2, Type> type = {Type::clamped, Type::open};
  glm::vec<2, GLushort> p = {2, 2};

  double dS = .01;
  double order = 2;

  glm::vec<2, GLuint> resolution = {20,20}; 


  Triangulation mode = Triangulation::uniform;

  std::vector<std::vector<vec3>> control_points;

  BsplineSurface(glm::vec<2, GLushort> n, glm::vec<2, GLushort> p, glm::vec<2, Type> type);

  BsplineSurface(const BezierSurface&);

  BsplineSurface(const std::vector<std::vector<vec3>>& control_points, glm::vec<2, GLushort> p, glm::vec<2, Type> type={Type::clamped, Type::clamped});

  Vertex evaluate(GLfloat u, GLfloat v) const;

  double cur_measure(double u, double v, double order) const;

  void set_control_point(int i, int j, vec3 point);

  virtual void update(ObjectData&) const override;

  static Vertex evaluate(
    const std::vector<std::vector<vec3>>& control_points, 
    glm::vec<2, GLfloat> t, 
    glm::vec<2, GLushort> p,
    glm::vec<2, Type> type
  );

};




#endif