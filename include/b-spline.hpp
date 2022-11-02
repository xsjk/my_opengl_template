#ifndef _B_SPLINE_HPP_
#define _B_SPLINE_HPP_
#include <defines.h>
#include <vertex.h>
#include <object_data_updater.h>
#include <bezier.h>

template<typename V=vec3>
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

  std::vector<V> control_points;
  GLushort p; // order of the spline

  GLuint resolution = 10;


  BsplineCurve(GLushort m, GLushort p=2, Type type=Type::clamped);
  BsplineCurve(const std::vector<V>& control_points, GLushort p=2, Type type = Type::clamped);

  void set_control_point(int i, V point);
  
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
  static Vertex evaluate(const std::vector<V>& control_points, GLushort p, GLfloat t, Type type=Type::clamped);

};


#include <sampler.hpp>

template<typename V=vec3>
class BsplineSurface : public ObjectDataUpdaterPlus {
  
  using Type = typename BsplineCurve<V>::Type;

public:
  
  glm::vec<2, Type> type = {Type::clamped, Type::open};
  glm::vec<2, GLushort> p = {2, 2};

  double dS = .01;
  double order = 2;

  glm::vec<2, GLuint> resolution = {20,20}; 


  Triangulation mode = Triangulation::adaptive;

  std::vector<std::vector<V>> control_points;

  BsplineSurface(glm::vec<2, GLushort> n, glm::vec<2, GLushort> p, glm::vec<2, Type> type);

  BsplineSurface(const std::vector<std::vector<V>>& control_points, glm::vec<2, GLushort> p, glm::vec<2, Type> type={Type::clamped, Type::clamped});

  Vertex evaluate(GLfloat u, GLfloat v) const;

  double cur_measure(double u, double v, double order) const;

  void set_control_point(int i, int j, V point);

  virtual void update(ObjectData&) const override;

  static Vertex evaluate(
    const std::vector<std::vector<V>>& control_points, 
    glm::vec<2, GLfloat> t, 
    glm::vec<2, GLushort> p,
    glm::vec<2, Type> type
  );

};

#include <utils.h>
#include <b-spline.h>
#include <delaunator.h>

template <typename V>
typename BsplineCurve<V>::Cache BsplineCurve<V>::cache;

template <typename V>
BsplineCurve<V>::BsplineCurve(GLushort m, GLushort p, Type t)
  : BsplineCurve{std::vector<V>{m},p,t} {}

template <typename V>
BsplineCurve<V>::BsplineCurve(
  const std::vector<V> &control_points,
  GLushort p,
  Type t
): 
  ObjectDataUpdaterPlus{GL_LINE_STRIP, false}, 
  control_points{control_points},
  type{t},
  p{p}
{
  if (control_points.size() < p+1) 
    throw std::invalid_argument("the number of control points must be at least p+1");
}

template <typename V>
void BsplineCurve<V>::set_control_point(int i, V point) {
  control_points[i] = point;
}


template <typename V>
Vertex BsplineCurve<V>::evaluate(const std::vector<V> &control_points, GLushort p, GLfloat t, Type type) {
  const auto n = control_points.size();
  const auto &c = BsplineCurve<V>::coeffs(n, p, t, type, 0),
             &dc= BsplineCurve<V>::coeffs(n, p, t, type, 1);
  const auto &pos= (weighted_sum(control_points, c)),
             &dir= (weighted_sum(control_points, dc));
  return Vertex{pos, dir};
}


template <typename V>
double BsplineCurve<V>::cur_measure(double t) const {
  const auto n = control_points.size();
  const auto &dc = BsplineCurve<V>::coeffs(n, p, t, type, 1),
             &ddc= BsplineCurve<V>::coeffs(n, p, t, type, 2);
  const auto &dir = (weighted_sum(control_points, dc)),
             &norm= (weighted_sum(control_points, ddc));
  return length(cross(dir,norm))/pow(length(dir),2);
}

template <typename V>
std::vector<GLfloat> BsplineCurve<V>::knot(GLushort n, GLushort k, Type type) {
  if(n < k+1) throw std::runtime_error(std::to_string(n) + " control points can at most generate curve of order " + std::to_string(n-1));
  std::vector<GLfloat> u;
  u.reserve(n+k+1);
  int i = 0;
  switch (type) {
    case Type::clamped:
      u.reserve(n+k+1);
      for (; i<k; ++i)    u.push_back(0);
      for (; i<=n; ++i)   u.push_back(float(i-k)/(n-k));
      for (; i<=n+k; ++i) u.push_back(1+1e-6);
      break;
    case Type::open:
      for (; i<=n+k; ++i) u.push_back(float(i-k)/(n-k));
      break;
    default:
      throw std::runtime_error("Unknown type");
  }
  return u;
}


template <typename V>
const std::vector<GLfloat> &BsplineCurve<V>::coeffs(GLushort p, GLfloat t, const std::vector<GLfloat> &u, GLushort k) {

  int n = u.size()-1-p;

  if (n <= 0)
    throw std::runtime_error("the knot vector is too short for basis function at degree " + std::to_string(p));

  auto &N = cache[u][{p,k,t}];
  if (!N.empty()) return N; 
  else N.resize(n);

  if ( k == 0 ) {
    if( p == 0 ) { 
      for (int i=0; i<n; ++i)
        N[i] = u[i] <= t && t < u[i+1];
    } else {
      const auto &N_ = coeffs(p-1, t, u, k);
      for (int i=0; i<n; ++i) {
        if (u[i+p]!=u[i])
          N[i] += (t-u[i])/(u[i+p]-u[i]) * N_[i];
        if(u[i+p+1]!=u[i+1])
          N[i] += (u[i+p+1]-t)/(u[i+p+1]-u[i+1]) * N_[i+1];
      }
    }
  } else {
    if ( p > 0 ) {
      const auto &N_ = coeffs(p-1, t, u, k-1); 
      for (int i=0; i<n; ++i) {
        if (u[i+p]!=u[i])
          N[i] += N_[i]/(u[i+p]-u[i]);
        if (u[i+p+1]!=u[i+1])
          N[i] -= N_[i+1]/(u[i+p+1]-u[i+1]);
        N[i] *= p;
      }
    }
  }
  return N;
}


template <typename V>
const std::vector<GLfloat> &BsplineCurve<V>::coeffs(GLushort n, GLushort p, GLfloat t, Type type, GLushort k) {
  switch(type) {
    case Type::clamped: 
      break;
    case Type::open:
      break;
    case Type::closed:
      break;
    default:
      throw std::runtime_error("Unknown type");
  }
  return coeffs(p, t, knot(n, p, type), k);
}


template <typename V>
Vertex BsplineCurve<V>::evaluate(GLfloat t) const {
  return evaluate(control_points, p, t, type);
}

#include <render_data.h>
#include <object_data.h>
#include <simpson.h>

template <typename V>
void BsplineCurve<V>::update(ObjectData &data) const {
  switch (mode) {
    case Triangulation::uniform:
      // normal case
      data.vertices().resize(resolution+1);  
      for (GLuint i=0; i<=resolution; ++i)
        data.vertices()[i] = evaluate(float(i)/resolution);
      break;
    case Triangulation::adaptive:
      // adaptive case
      data.vertices().clear();
      std::function<double(double)> f = [this](double t) { return cur_measure(t); };
      for(auto t: Simpson::partition(f, 0.5, 0, 1))
        data.vertices().push_back(evaluate(t));
  }
}


template <typename V>
BsplineSurface<V>::BsplineSurface(glm::vec<2, GLushort> n, glm::vec<2, GLushort> p, glm::vec<2, Type> type)
  : BsplineSurface{std::vector<std::vector<V>>(n[0], std::vector<V>(n[1])), p, type} {}

template <typename V>
BsplineSurface<V>::BsplineSurface(
  const std::vector<std::vector<V>> &control_points,
  glm::vec<2, GLushort> p, glm::vec<2, Type> type
): 
  ObjectDataUpdaterPlus{GL_TRIANGLES, true}, 
  control_points{control_points}, p{p}, type{type}
{}

template <typename V>
void BsplineSurface<V>::set_control_point(int i, int j, V point) {
  control_points[i][j] = point;
}



template <typename V>
Vertex BsplineSurface<V>::evaluate(
  const std::vector<std::vector<V>> &control_points,
  glm::vec<2, GLfloat> t, glm::vec<2, GLushort> p, glm::vec<2,Type> type
) {
  glm::vec<2,GLuint> n {control_points.size(),control_points.front().size()};

  const auto &cu  = BsplineCurve<V>::coeffs(n[0], p[0], t[0], type[0], 0),
             &cv  = BsplineCurve<V>::coeffs(n[1], p[1], t[1], type[1], 0),
             &dcu = BsplineCurve<V>::coeffs(n[0], p[0], t[0], type[0], 1),
             &dcv = BsplineCurve<V>::coeffs(n[1], p[1], t[1], type[1], 1);

  const auto &pos = weighted_sum(control_points, cu, cv),
             &du  = weighted_sum(control_points, dcu, cv),
             &dv  = weighted_sum(control_points, cu, dcv);

  auto norm = evalue(cross(du,dv));
  if (norm==V(0)) {
    norm = evaluate(control_points, t+(.5f-t)*.001f, p, type).normal;
    std::cout << "Warning: normal is zero at " << t << '\t' << norm << std::endl;
    norm = evalue(norm);
  }
    
  return {pos, norm, t};
}


template <typename V>
Vertex BsplineSurface<V>::evaluate(GLfloat u, GLfloat v) const {
  return evaluate(control_points, {u, v}, p, type);
}

template <typename V>
double BsplineSurface<V>::cur_measure(double u, double v, double order) const {

  glm::vec<2,GLuint> n {control_points.size(),control_points.front().size()};

  const auto &cu  = BsplineCurve<V>::coeffs(n[0], p[0], u, type[0], 0),
             &cv  = BsplineCurve<V>::coeffs(n[1], p[1], v, type[1], 0),
             &dcu = BsplineCurve<V>::coeffs(n[0], p[0], u, type[0], 1),
             &dcv = BsplineCurve<V>::coeffs(n[1], p[1], v, type[1], 1),
             &ddcu= BsplineCurve<V>::coeffs(n[0], p[0], u, type[0], 2),
             &ddcv= BsplineCurve<V>::coeffs(n[1], p[1], v, type[1], 2);

  const auto &du  = weighted_sum(control_points, dcu, cv),
             &dv  = weighted_sum(control_points, cu, dcv),
             &ddu = weighted_sum(control_points, ddcu, cv),
             &ddv = weighted_sum(control_points, cu, ddcv);
  
  auto norm = evalue(cross(du,dv));
  auto measure_u = length(cross(du,ddu))/(pow(length(du), order)),
       measure_v = length(cross(dv,ddv))/(pow(length(dv), order));

  // this must not return nan
  auto res = (measure_u + measure_v)/2 + measure_u * measure_v;

  if (std::isnan(res))
    throw;

  return res;
}


template <typename V>
void BsplineSurface<V>::update(ObjectData &data) const {
  switch (mode) {
    case Triangulation::adaptive: {
      AdaptiveSampler<BsplineSurface<V>> s{*this};
      data.vertices() = std::move(s.vertices);
      data.indices() = std::move(s.mesh_indices);
    } break;
    case Triangulation::uniform: {
      Sampler<BsplineSurface<V>> s{*this};
      data.indices() = std::move(s.mesh_indices);
      data.vertices() = std::move(s.vertices);
    } break;
  }
  return;
}




#endif