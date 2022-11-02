#ifndef _HANDLER_HPP_
#define _HANDLER_HPP_

#include <group.h>

template<class D>
struct Handler : public std::shared_ptr<D> {

  /// @brief enable construction like the handled object 
  template<class ...Args, class = std::enable_if_t<std::is_constructible_v<D, Args...>>>
  Handler(Args&&... args) : std::shared_ptr<D>{std::make_shared<D>(std::forward<Args>(args)...)} {}

  /// @brief enable polymophism  
  template<class T, class = std::enable_if_t<std::is_base_of_v<D, T>>>
  inline Handler(const std::shared_ptr<T> &ptr) : std::shared_ptr<D>{ptr} {}
  template<class T, class = std::enable_if_t<std::is_base_of_v<D, T>>>
  inline Handler(std::shared_ptr<T> &&ptr) : std::shared_ptr<D>{std::move(ptr)} {}
  template<class T, class = std::enable_if_t<std::is_base_of_v<D, T>>>
  inline Handler(T&& o) : std::shared_ptr<D>{std::make_shared<T>(std::forward<T>(o))} {}

  // /// @brief enable calling like the handled object
  // template<class ...Args>
  // inline auto operator()(Args&&... args) const {
  //   return (*this)->operator()(std::forward<Args>(args)...);
  // }
  
  inline Handler copy() { return Handler{**this}; }

  template<class T>
  inline Handler& operator=(T&& other) {
    std::shared_ptr<D>::operator=(std::forward<T>(other));
    return *this;
  }
};


namespace std {
  template<class D>
  struct hash<Handler<D>> {
    inline size_t operator()(const Handler<D>& h) const {
      return std::hash<std::shared_ptr<D>>()(h);
    }
  };
}

#endif