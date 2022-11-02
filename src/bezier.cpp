#include <utils.h>
#include <bezier.h>
#include <sampler.hpp>
#include <delaunator.h>


BezierCurve::Cache BezierCurve::cache;

BezierCurve::BezierCurve(int m) : ObjectDataUpdaterPlus{GL_LINE_STRIP, false} {
  control_points.resize(m);
}

BezierCurve::BezierCurve(const std::vector<vec3> &control_points) : ObjectDataUpdaterPlus{GL_LINE_STRIP, false}, control_points{control_points} {}

void BezierCurve::set_control_point(int i, vec3 point) {
  control_points[i] = point;
}

Vertex BezierCurve::evaluate(const std::vector<vec3> &control_points, GLfloat t) {
  const auto n = control_points.size();
  const auto &c =  BezierCurve::coeffs(n-1, t),
            &dc =  BezierCurve::coeffs(n-1, t, 1);
  const auto &p = weighted_sum(control_points, c);
  const auto &dp = weighted_sum(control_points, dc);
  return {p, dp, {t,0}};
}

double BezierCurve::cur_measure(double t) const {
  const auto  n = control_points.size()-1;
  const auto &dc = coeffs(n,t,1),
             &ddc = coeffs(n,t,2);
  const auto &dir = evalue(weighted_sum(control_points, dc)),
             &norm = evalue(weighted_sum(control_points, ddc));
  return length(cross(dir,norm))/pow(length(dir),2);
}

const std::vector<GLfloat> &BezierCurve::coeffs(GLushort p, GLfloat t, GLushort k) {
  auto &res = cache[{p,k,t}];
  if (!res.empty()) return res;
  else res.resize(p+1);
  if (k==0) {
    if (p==0) {
      res[0] = 1;
    } else {
      const auto &c = coeffs(p-1,t,0);
      for (int i=0; i<p; ++i)
        res[i] = (1-t)*c[i];
      for (int i=1; i<=p; ++i)
        res[i] += t*c[i-1];
    }
  } else {
    if (p>0) {
      const auto &c = coeffs(p-1,t,k-1);
      res[0] = -c[0]  *p;
      res[p] = c[p-1]*p;
      for (int i=1; i<p; ++i)
        res[i] = (c[i-1] - c[i]) * p;
    }
  }
  return res;
}

Vertex BezierCurve::evaluate(GLfloat t) const {
  return evaluate(control_points, t);
}

#include <render_data.h>
#include <object_data.h>

void BezierCurve::update(ObjectData &data) const {
  switch(mode) {
    case Triangulation::adaptive: {
      data.vertices().clear();
      std::function<double(double)> f = [this](double t) { return cur_measure(t); };
      for(auto t: Simpson::partition(f, 0.5, 0, 1))
        data.vertices().push_back(evaluate(t));  
    } break;
    case Triangulation::uniform: {
      data.vertices().resize(resolution+1);  
      for (GLuint i=0; i<=resolution; ++i)
        data.vertices()[i] = evaluate(float(i)/resolution);
    } break;
  }  
}


BezierSurface::BezierSurface(int m, int n) 
  : ObjectDataUpdaterPlus{GL_TRIANGLES, true},
    control_points(m, std::vector<vec3>(n)) {}

BezierSurface::BezierSurface(const std::vector<std::vector<vec3>> &control_points) 
  : ObjectDataUpdaterPlus{GL_TRIANGLES, true} , control_points{control_points} {}


Vertex BezierSurface::evaluate(const std::vector<std::vector<vec3>> &control_points,
                               GLfloat u, GLfloat v) {
  const auto n = control_points.size()-1,
             m = control_points.front().size()-1;

  const auto &cu =  BezierCurve::coeffs(n, u, 0),
             &cv =  BezierCurve::coeffs(m, v, 0),
             &dcu = BezierCurve::coeffs(n, u, 1),
             &dcv = BezierCurve::coeffs(m, v, 1);

  const auto &pos = weighted_sum(control_points, cu, cv),
             &du  = weighted_sum(control_points, dcu, cv),
             &dv  = weighted_sum(control_points, cu, dcv);

  auto norm = evalue(cross(du,dv));
  if (norm==vec3(0)) norm = evalue(evaluate(control_points, u+(.5f-u)*.001f,  v+(.5f-v)*.001f).normal);
    
  return {pos, norm, {u,v}};

}

Vertex BezierSurface::evaluate(GLfloat u, GLfloat v) const {
  return evaluate(control_points, u, v);
}

double BezierSurface::cur_measure(double u, double v, double order) const {
  if (.5-std::abs(u-.5)<.001 || .5-std::abs(v-.5)<.001)
    return cur_measure(u+(.5-u)*.001,  v+(.5-v)*.001, order);
    
  const auto p = control_points.size()-1,
             q = control_points.front().size()-1;

  const auto &cu   = BezierCurve::coeffs(p, u),
             &cv  =  BezierCurve::coeffs(q, v),
             &dcu  = BezierCurve::coeffs(p, u, 1),
             &dcv  = BezierCurve::coeffs(q, v, 1),
             &ddcu = BezierCurve::coeffs(p, u, 2),
             &ddcv = BezierCurve::coeffs(q, v, 2);

  const auto &du  = weighted_sum(control_points, dcu, cv),
             &dv  = weighted_sum(control_points, cu, dcv),
             &ddu = weighted_sum(control_points, ddcu, cv),
             &ddv = weighted_sum(control_points, cu, ddcv);


  auto norm = evalue(cross(du,dv));
  auto measure_u = length(cross(du,ddu))/(pow(length(du), order)),
       measure_v = length(cross(dv,ddv))/(pow(length(dv), order));

  auto res = (measure_u + measure_v)/2 + measure_u * measure_v;

  if(std::isnan(res)) {
    res = cur_measure(u+(.5-u)*.001, v+(.5-v)*.001, order);
  }

  return res;
}

#include <simpson.h>

void BezierSurface::update(ObjectData &data) const {
  switch (mode) {
    case Triangulation::adaptive: {
      AdaptiveSampler<BezierSurface> s{*this};
      data.vertices() = std::move(s.vertices);
      data.indices() = std::move(s.mesh_indices);
    } break;
    case Triangulation::uniform: {
      Sampler<BezierSurface> s{*this};
      data.indices() = std::move(s.mesh_indices);
      data.vertices() = std::move(s.vertices);
    } break;
  }
}

std::vector<BezierSurface> BezierSurface::read(const std::string &path) {
    std::ifstream file(getPath(path));
    GLuint num_surfaces,num_control_points,m,n;
    file >> num_surfaces >> num_control_points >> m >> n;
    std::vector<BezierSurface> result;
    result.reserve(num_surfaces);
    std::vector<std::vector<std::vector<GLuint>>> indices(num_surfaces);
    std::vector<vec3> position(num_control_points);
    for(GLuint i = 0 ; i < num_surfaces ; ++i) {
      indices[i].resize(m);
      for(GLuint j = 0 ; j < m ; ++j) {
        indices[i][j].resize(n);
        for(GLuint k = 0 ; k < n ; ++k)
          file>>indices[i][j][k];
      }
    }
    for(GLuint i = 0 ; i < num_control_points ; ++i)
      file>> position[i].x>> position[i].y >>position[i].z;
    for(GLuint i = 0 ; i < num_surfaces ; ++i) {
      BezierSurface s(m,n);
      for(GLuint j = 0 ; j < m ; ++j) 
        for(GLuint k = 0 ; k < n ; ++k)
          s.control_points[j][k] = position[indices[i][j][k]];
      result.push_back(s);
    }
    return result;
}


