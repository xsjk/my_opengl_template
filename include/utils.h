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


static const std::vector<GLuint>& Binomial(GLuint n) {
  static std::vector<std::vector<GLuint>> coeff_lists = {{1}, {1, 1}};
  if (coeff_lists.size() <= n) [[unlikely]] {
    const std::vector<GLuint>& prev = Binomial(n - 1);
    std::vector<GLuint> next(n + 1);
    next[0] = next[n] = 1;
    for (GLuint i = 1; i < n; ++i)
      next[i] = prev[i - 1] + prev[i];
    return coeff_lists.emplace_back(std::move(next));
  }
  return coeff_lists[n];
}

template<typename T>
T sum(std::function<T(int)> f, int start, int end) {
  T res = 0;
  for (int i = start; i <= end; ++i)
    res += f(i);
  return res;
} 


/// @brief weighted sum of v by k
/// @param v: the values (1d vector)
/// @param w: the weights (1d vector)
/// @return the sum of w[i]*v[i]
template<typename T, typename W>
inline T weighted_sum(
  const std::vector<T> v,
  const std::vector<W> w
) {
  T sum{0};
  for (unsigned i = 0; i < w.size(); ++i)
    sum += w[i] * v[i];
  return sum;
}

/// @brief weighted sum of v at w1@w2
/// @param v: the values (2d vector)
/// @param w1: the weights (1d vector)
/// @param w2: the weights (1d vector)
/// @return the sum of w1[i]*w2[j]*v[i][j]
template<typename T, typename W>
inline T weighted_sum(
  const std::vector<std::vector<T>>& v,
  const std::vector<W>& w1, 
  const std::vector<W>& w2
) {
  T sum{0};
  for (unsigned i = 0; i < w1.size(); ++i)
    for (unsigned j = 0; j < w2.size(); ++j)
      sum += w1[i] * w2[j] * v[i][j];
  return sum;
}



// ignore float that < 1e-5
template<typename T, typename U=std::enable_if_t<std::is_arithmetic_v<T>>>
inline T evalue(T x){
  return x < static_cast<T>(1e-5) && x > -static_cast<T>(1e-5) ? 0 : x;
}

template<typename T>
inline glm::vec<3,T> evalue(glm::vec<3,T> x) {
  for (int i=0; i<3; ++i) x[i] = evalue(x[i]);
  return x;
}


template<int N, int M, typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::mat<N, M, T>& mat) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j)
      os << mat[i][j] << " ";
    os << std::endl;
  }
  return os;
}

/// hash function for std::pair
namespace std {
  template<typename T1, typename T2>
  struct hash<std::pair<T1, T2>> {
    size_t operator()(const std::pair<T1, T2>& p) const {
      return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
    }
  };

  template<typename T>
  struct hash<std::vector<T>> {
    size_t operator()(const std::vector<T>& v) const {
      size_t seed = v.size();
      for (auto& i : v)
        seed ^= std::hash<T>()(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }
  };
}

template<typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
  return os << "(" << p.first << ", " << p.second << ")";
}

template<int N, typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::vec<N, T>& v) {
  os << "(";
  for (int i = 0; i < N; ++i)
    os << v[i] << (i == N - 1 ? ")" : ", ");
  return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
  if (v.empty()) return os << "[]";
  os << "[";
  for (const auto& e : v) os << e << ", ";
  return os << "\b\b]";
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& v) {
  if (v.empty()) return os << "{}";
  os << "{";
  for (const auto& e : v) os << e << ", ";
  return os << "\b\b}";
}

template<typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<T1, T2>& v) {
  if (v.empty()) return os << "{}";
  os << "{";
  for (const auto& e : v) os << e.first << ": " << e.second << ", ";
  return os << "\b\b}";
}

#endif