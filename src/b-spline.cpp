
// std::vector<BsplineSurface<T>> BsplineSurface::read(const std::string &path) {
//     std::ifstream file(getPath(path));
//     GLuint num_surfaces,num_control_points,m,n;
//     file >> num_surfaces >> num_control_points >> m >> n;
//     std::vector<BsplineSurface> result;
//     result.reserve(num_surfaces);
//     std::vector<std::vector<std::vector<GLuint>>> indices(num_surfaces);
//     std::vector<T> position(num_control_points);
//     for(GLuint i = 0 ; i < num_surfaces ; ++i) {
//       indices[i].resize(m);
//       for(GLuint j = 0 ; j < m ; ++j) {
//         indices[i][j].resize(n);
//         for(GLuint k = 0 ; k < n ; ++k)
//           file>>indices[i][j][k];
//       }
//     }
//     for(GLuint i = 0 ; i < num_control_points ; ++i)
//       file>> position[i].x>> position[i].y >>position[i].z;
//     for(GLuint i = 0 ; i < num_surfaces ; ++i) {
//       BsplineSurface s(m,n);
//       for(GLuint j = 0 ; j < m ; ++j) 
//         for(GLuint k = 0 ; k < n ; ++k)
//           s.set_control_point(j,k,position[indices[i][j][k]]);
//       result.push_back(s);
//     }
//     return result;
// }
