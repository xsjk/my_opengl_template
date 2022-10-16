#ifndef _UTILS_H_
#define _UTILS_H_
#include <defines.h>


bool isNumber(std::string token);
/**
 * try and search target file in parent directories with max depth limit
 */
std::string getPath(const std::string &target, int depth = 5);

struct WindowGuard final {
  WindowGuard(GLFWwindow *&, const int width, const int height, const std::string &title);
  ~WindowGuard();
};



template<typename T, typename K>
inline T tensor_product_sum(
  const std::vector<std::vector<T>>& c,
  const std::vector<K>& a, 
  const std::vector<K>& b
) {
  T res{0};
  for (unsigned i = 0; i < a.size(); ++i)
    for (unsigned j = 0; j < b.size(); ++j)
      res += a[i] * b[j] * c[i][j];
  return res;
}


template<int N, int M, typename T>
inline void print(const glm::mat<N, M, T>& mat) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j)
      std::cout << mat[i][j] << " ";
    std::cout << std::endl;
  }
}

#endif