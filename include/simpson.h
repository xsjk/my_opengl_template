#ifndef _SIMPSON_H_
#define _SIMPSON_H_

#include <utils.h>

namespace Simpson {

  double integrate(
    const std::function<double(double)>& f, 
    const double l, const double r, 
    const double eps = 1e-6
  );

  double integrate(
    const std::function<double(double,double)>& f, 
    const double l1, const double r1, 
    const double l2, const double r2, 
    const double eps = 1e-6);


  /// @brief calculate the integral of a function f(x) in the interval [l,r] and call the callback function for each subinterval
  /// @note the callback function should return whether the recursion should continue!
  std::set<double> partition(
    const std::function<double(double)>& f, 
    double unit,
    const double l, const double r, 
    const double eps = 1e-6
  );
  

  /// @brief calculate the integral of a function f(x,y) in the interval [l1,r1]x[l2,r2] and call the callback function for each subinterval
  /// @note the callback function should return whether the recursion should continue!
  std::unordered_set<std::pair<double,double>> partition(
    const std::function<double(double,double)>& f, 
    double unit,
    const double l1, const double r1, 
    const double l2, const double r2, 
    const double eps = 1e-6
  );


  std::unordered_set<std::pair<double,double>> fast_partition(
    const std::function<double(double,double)>& f, 
    double unit,
    const double l1, const double r1, 
    const double l2, const double r2
  );

}


#endif
