#include <utils.h>
#include <bezier.h>


BezierCurve::BezierCurve(int m) : RenderDataUpdater{GL_LINE_STRIP, false} {
  control_points_.resize(m);
}

BezierCurve::BezierCurve(const std::vector<vec3>& control_points) : RenderDataUpdater{GL_LINE_STRIP, false}, control_points_{control_points} {}

void BezierCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

/**
 * TODO: evaluate the point at t with the given control_points
 */
Vertex BezierCurve::evaluate(const std::vector<vec3>& control_points, GLfloat t) {
  // std::vector<vec3> points = control_points;
  // GLuint n = points.size();
  // GLfloat _t = 1 - t;
  // for (int i = 1; i < n - 1; ++i) for (int j = 0; j < n - i; ++j)
  //   points[j] = _t * points[j] + t * points[j + 1];  
  // return Vertex{_t*points[0] + t*points[1], points[1]-points[0]};
  int n = control_points.size();
  auto c =  BezierCurve::coeff(n-1, t);
  auto dc = BezierCurve::dcoeff(c, t);
  vec3 p(0), d(0);
  for (int i = 0; i < n; ++i) {
    p +=  c[i] * control_points[i];
    d += dc[i] * control_points[i];
  }
  return Vertex{p, d};
}



std::vector<GLfloat> BezierCurve::coeff(GLuint n, GLfloat t) {
  std::vector<GLfloat> res(n+1);
  GLfloat _t = 1-t;
  std::vector<GLuint> B(n+1);  // binomial coefficients
  std::vector<GLfloat> T(n+1), _T(n+1);
  T[0] = _T[0] = B[0] = 1;
  for (GLuint i=1; i<=n; ++i) {
     T[i] =  T[i-1]* t; 
    _T[i] = _T[i-1]*_t; 
     B[i] = 1; 
    for (GLuint j=i-1; j>0; --j) 
      B[j] += B[j-1];
  }
  for (GLuint i=0; i<=n; i++)
    res[i] = B[i] * T[i] * _T[n-i];
  return res;

}

std::vector<GLfloat> BezierCurve::dcoeff(const std::vector<GLfloat>& coeff, GLfloat t) {
  GLuint n = coeff.size()-1;
  std::vector<GLfloat> res;
  if (t==0) {
    res.resize(n+1);
    res[0] = -1;
    res[1] = 1;
  } else if (t==1) {
    res.resize(n+1);
    res[n-1] = -1;
    res[n] = 1;
  } else {
    res = coeff;
    for (GLuint i=0; i<=n; ++i)
      res[i] *= (i/t - (n-i)/(1-t))/n;
  }
  return res;  
}


Vertex BezierCurve::evaluate(GLfloat t) const {
  return evaluate(control_points_, t);
}

std::vector<GLfloat> BezierCurve::coeff(GLfloat t) const {
  return coeff(control_points_.size()-1, t);
}


void BezierCurve::operator()(std::vector<Vertex>& vertices, std::vector<GLuint>&) const {
  vertices.resize(resolution+1);  
  for (GLuint i=0; i<=100; ++i)
    vertices[i] = evaluate(float(i)/resolution);
}


BezierSurface::BezierSurface(int m, int n) 
  : RenderDataUpdater{GL_TRIANGLES, true} {
  control_points_m_.resize(m);
  for (auto& sub_vec : control_points_m_) sub_vec.resize(n);
  control_points_n_.resize(n);
  for (auto& sub_vec : control_points_n_) sub_vec.resize(m);
}

BezierSurface::BezierSurface(const std::vector<std::vector<vec3>>& control_points) 
  : RenderDataUpdater{GL_TRIANGLES, true} {
  control_points_m_ = control_points;
  control_points_n_.resize(control_points[0].size());
  for (auto& sub_vec : control_points_n_)
    sub_vec.resize(control_points.size());
  for (GLuint i=0; i<control_points.size(); ++i)
    for (GLuint j=0; j<control_points[i].size(); ++j)
      control_points_n_[j][i] = control_points[i][j];
}

/**
 * @param[in] i: index (i < m)
 * @param[in] j: index (j < n)
 * @param[in] point: the control point with index i, j
 */
void BezierSurface::setControlPoint(int i, int j, vec3 point) {
  control_points_m_[i][j] = point;
  control_points_n_[j][i] = point;
}

/**
 * TODO: evaluate the point at (u, v) with the given control points
 */
Vertex BezierSurface::evaluate(const std::vector<std::vector<vec3>>& control_points,
                               GLfloat u, GLfloat v) {
  GLuint n = control_points.size()-1;
  GLuint m = control_points.front().size()-1;

  auto cu =  BezierCurve::coeff(n, u),
       cv =  BezierCurve::coeff(m, v),
       dcu = BezierCurve::dcoeff(cu, u),
       dcv = BezierCurve::dcoeff(cv, v);

  auto pos = tensor_product_sum(control_points, cu, cv),
       du  = tensor_product_sum(control_points, dcu, cv),
       dv  = tensor_product_sum(control_points, cu, dcv);

  return Vertex{pos, cross(du, dv)};

}

Vertex BezierSurface::evaluate(GLfloat u, GLfloat v) const {
  return evaluate(control_points_m_, u, v);
}


void BezierSurface::operator()(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) const {
  vertices.resize((resolution_m+1)*(resolution_n+1));
  for (GLuint i=0; i<=resolution_m; ++i) {
    for (GLuint j=0; j<=resolution_n; ++j) {
      vertices[i*(1+resolution_n)+j] = evaluate(float(i)/resolution_m, float(j)/resolution_n);
    }
  }

  indices.resize(resolution_m*resolution_n*6);
  GLuint index_offset = 0;
  for (GLuint i=0; i<resolution_m; ++i) {
    for (GLuint j=0; j<resolution_n; ++j) {
      GLuint index[4] = {
            i*(resolution_n+1)+j,
            i*(resolution_n+1)+j+1,
        (i+1)*(resolution_n+1)+j,
        (i+1)*(resolution_n+1)+j+1,
      };
      indices[index_offset++] = index[0];
      indices[index_offset++] = index[1];
      indices[index_offset++] = index[2];
      indices[index_offset++] = index[1];
      indices[index_offset++] = index[3];
      indices[index_offset++] = index[2];
    }
  }
  return;

  
}


/**
 * TODO: read in bzs file to generate a vector of Bezier surface
 * for the first line we have b p m n
 * b means b bezier surfaces, p means p control points.
 *
 */
std::vector<BezierSurface> BezierSurface::read(const std::string &path) {
    std::ifstream file(getPath(path));
    GLuint num_surfaces,num_control_points,m,n;
    file >> num_surfaces >> num_control_points >> m >> n;
    std::vector<BezierSurface> result;
    result.reserve(num_surfaces);
    std::vector<std::vector<std::vector<unsigned int>>> indices(num_surfaces);
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
      BezierSurface tmp(m,n);
      for(GLuint j = 0 ; j < m ; ++j) {
        for(GLuint k = 0 ; k < n ; ++k) {
          tmp.setControlPoint(j,k,position[indices[i][j][k]]);
        }
      }
      result.push_back(tmp);
    }
    return result;
}
