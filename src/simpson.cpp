#include <simpson.h>


namespace Simpson {

  const int max_depth = 12;


  template<typename X, typename Y>
  inline Y simpson(const std::function<Y(X)>& f, const X l, const X r) {
    return (r-l)/6*(f(l)+4*f((l+r)/2)+f(r));
  }
  
  template<typename X, typename Y, typename Z>
  inline Z simpson(const std::function<Z(X,Y)>& f, const X l, const X r, const Y u, const Y d) {
    // std::cout << mat3{
    //   f(l,u), f((l+r)/2,u), f(r,u),
    //   f(l,(u+d)/2), f((l+r)/2,(u+d)/2), f(r,(u+d)/2),
    //   f(l,d), f((l+r)/2,d), f(r,d)
    // } << std::endl;
    return (r-l)*(d-u)/36*(f(l,u)+f(l,d)+f(r,u)+f(r,d)+4*(f((l+r)/2,u)+f((l+r)/2,d)+f(l,(u+d)/2)+f(r,(u+d)/2))+16*f((l+r)/2,(u+d)/2));
  }

  template<typename X, typename Y>
  Y adpative_simpson(const std::function<Y(X)>& f, const X l, const X r, const Y eps, const Y S) {
    const X m = (l + r) / 2;
    const Y L = simpson(f, l, m);
    const Y R = simpson(f, m, r);
    const Y E = (L + R - S) / 15;
    if (std::abs(E) <  eps) return S + E;
    else return adpative_simpson(f, l, m, eps/2, L) + adpative_simpson(f, m, r, eps/2, R);
  }  

  template<typename X, typename Y>
  Y adpative_simpson(const std::function<Y(X)>& f, const X l, const X r, const Y eps, const Y S, const std::function<bool(X,X,Y)>& callback) {
    const X m = (l + r) / 2;
    const Y L = simpson(f, l, m);
    const Y R = simpson(f, m, r);
    const Y E = (L + R - S) / 15;
    Y res;
    if (std::abs(E) <  eps)
      res = S + E;
    else 
      res = adpative_simpson(f, l, m, eps/2, L, callback) + adpative_simpson(f, m, r, eps/2, R, callback);
    if (callback(l, r, res))
      res = adpative_simpson(f, l, m, eps/2, L, callback) + adpative_simpson(f, m, r, eps/2, R, callback);
    return res;
  }

  template<typename X, typename Y, typename Z>
  Z adpative_simpson(const std::function<Z(X,Y)>& f, const X l, const X r, const Y u, const Y d, const Z eps, const Z S, const int depth=0) {
    const X mh = (l + r) / 2;
    const Y mv = (u + d) / 2;
    const Z LU = simpson(f, l, mh, u, mv);
    const Z RU = simpson(f, mh, r, u, mv);
    const Z LD = simpson(f, l, mh, mv, d);
    const Z RD = simpson(f, mh, r, mv, d);
    const Z E = (LU + RU + LD + RD - S) / 15 * 15;
    if (depth >= max_depth) return S + E;
    if (std::abs(E) < eps) return S + E;
    return adpative_simpson(f, l, mh, u, mv, eps/4, LU, depth+1)
         + adpative_simpson(f, mh, r, u, mv, eps/4, RU, depth+1)
         + adpative_simpson(f, l, mh, mv, d, eps/4, LD, depth+1)
         + adpative_simpson(f, mh, r, mv, d, eps/4, RD, depth+1);
  }

  // double integral with callback
  template<typename X, typename Y, typename Z>
  Z adpative_simpson(const std::function<Z(X,Y)>& f, const X l, const X r, const Y u, const Y d, const Z eps, const Z S, const std::function<bool(X,X,Y,Y,Z)>& callback, const int depth=0) {
    const X mh = (l + r) / 2;
    const Y mv = (u + d) / 2;
    const Z LU = simpson(f, l, mh, u, mv);
    const Z RU = simpson(f, mh, r, u, mv);
    const Z LD = simpson(f, l, mh, mv, d);
    const Z RD = simpson(f, mh, r, mv, d);
    const Z E = (LU + RU + LD + RD - S) / 15;
    Z res;
    if (depth >= max_depth) {
      res = S + E;
    } else
    if (std::abs(E) < eps) {
      res = S + E;
    } 
    else
      res = adpative_simpson(f, l, mh, u, mv, eps/4, LU, callback, depth+1)
          + adpative_simpson(f, mh, r, u, mv, eps/4, RU, callback, depth+1)
          + adpative_simpson(f, l, mh, mv, d, eps/4, LD, callback, depth+1)
          + adpative_simpson(f, mh, r, mv, d, eps/4, RD, callback, depth+1);
    if (callback(l, r, u, d, res))
      res = adpative_simpson(f, l, mh, u, mv, eps/4, LU, callback, depth+1)
          + adpative_simpson(f, mh, r, u, mv, eps/4, RU, callback, depth+1)
          + adpative_simpson(f, l, mh, mv, d, eps/4, LD, callback, depth+1)
          + adpative_simpson(f, mh, r, mv, d, eps/4, RD, callback, depth+1);
    return res;
  }


  double integrate(const std::function<double(double)>& f, const double l, const double r, const double eps) {
    return adpative_simpson(f, l, r, eps, simpson(f, l, r));
  }

  double integrate(const std::function<double(double,double)>& f, const double l, const double r, const double u, const double d, const double eps) {
    return adpative_simpson(f, l, r, u, d, eps, simpson(f, l, r, u, d));
  }

  // use the callback version of adpative_simpson, accumulate but slow
  std::set<double> partition(
    const std::function<double(double)>& f, 
    double unit, 
    const double l, const double r, const double eps
  ) {
    std::set<double> res = {l, r};
    const std::function<bool(double,double,double)> callback = [&res, unit](double l, double r, double S) {
      if(std::abs(S) > unit) { 
        res.insert((l+r)/2); 
        return true; 
      } else return false;
    };
    adpative_simpson(f, l ,r, eps, simpson(f, l, r), callback);
    return res;
  }

  template<typename X, typename Y, typename Z>
  Z adpative_partition(const std::function<Z(X,Y)>& f, std::unordered_set<std::pair<double,double>>& sample, const Z unit, const X l, const X r, const Y u, const Y d, const Z S, const Z eps, unsigned depth=0) {
    const X mh = (l + r) / 2;
    const Y mv = (u + d) / 2;
    const Z LU = simpson(f, l, mh, u, mv);
    const Z RU = simpson(f, mh, r, u, mv);
    const Z LD = simpson(f, l, mh, mv, d);
    const Z RD = simpson(f, mh, r, mv, d);
    const Z E = (LU + RU + LD + RD - S) / 15;
    Z res;
    if (depth >= max_depth) {
      // std::cerr << "max depth reached" << std::endl;
      res = S + E;
    } else 
    if (std::abs(S+E) > unit || std::abs(E) > eps) {
      res = adpative_partition(f, sample, unit, l, mh, u, mv, LU, eps/4, depth+1)
          + adpative_partition(f, sample, unit, mh, r, u, mv, RU, eps/4, depth+1)
          + adpative_partition(f, sample, unit, l, mh, mv, d, LD, eps/4, depth+1)
          + adpative_partition(f, sample, unit, mh, r, mv, d, RD, eps/4, depth+1);
    }
    if (std::abs(S+E) > unit) {
      sample.insert({(l+r)/2, (u+d)/2});
      sample.insert({(l+r)/2, u});
      sample.insert({(l+r)/2, d});
      sample.insert({l, (u+d)/2});
      sample.insert({r, (u+d)/2});
    }
    return res;
  }

// use the callback version of adpative_simpson, accumulate but slow
  std::unordered_set<std::pair<double,double>> partition(
    const std::function<double(double,double)>& f, 
    double unit, 
    const double l, const double r, const double u, const double d, const double eps
  ) {
    std::unordered_set<std::pair<double,double>> res = {{l, u}, {l, d}, {r, u}, {r, d}};
    const std::function<bool(double,double,double,double,double)> callback = [&res, unit](double l, double r, double u, double d, double S) {
      if(std::abs(S) > unit) { 
        res.insert({(l+r)/2, (u+d)/2});
        res.insert({(l+r)/2, u});
        res.insert({(l+r)/2, d});
        res.insert({l, (u+d)/2});
        res.insert({r, (u+d)/2}); 
        return true; 
      } else {
        return false;
      }
    };
    adpative_simpson(f, l ,r, u, d, eps, simpson(f, l, r, u, d), callback);
    return res;
  }

  std::unordered_set<std::pair<double,double>> fast_partition(
    const std::function<double(double,double)>& f, 
    double unit, 
    const double l, const double r, const double u, const double d
  ) {
    std::unordered_set<std::pair<double,double>> res = {{l, u}, {l, d}, {r, u}, {r, d}};
    adpative_partition(f, res, unit, l, r, u, d, simpson(f, l, r, u, d), unit);
    return res;
  }
  

}