#ifndef _WIND_H
#define _WIND_H

#include <defines.h>

struct WindField {
  virtual vec3 get_wind(vec3 position) {
    return vec3(0);
  }
};

struct myWindField : WindField {
  virtual vec3 get_wind(vec3 position) {
    return vec3(0);
  }
};


#endif