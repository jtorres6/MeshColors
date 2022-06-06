/*
* Practicas de IG
* Domingo Martin Perandres© 2014-2018
* dmartin@ugr.es
*
* GPL 3
*/

#ifndef AXIS_H
#define AXIS_H

#include "basic_object3d.h"

namespace _axis_ne {

  const float DEFAULT_AXIS_SIZE=5000;
}

class _axis:public _basic_object3D
{
public:
  _axis();
  _axis(float Size);
};

#endif
