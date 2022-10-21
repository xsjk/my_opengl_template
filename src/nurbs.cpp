
#include <nurbs.h>


void NURBSCurve::set_control_point(int i, vec3 point) {
  for (int j=0; j<3; ++j) control_points[i][j] = point[j];
}

void NURBSCurve::set_weight(int i, GLfloat w) {
  control_points[i][3] = w;
}

// void NURBSCurve::update(ObjectData &data) const {}

// Vertex NURBSCurve::evaluate(GLfloat t) const {

// }


// double NURBSCurve::cur_measure(double t) const {

// }