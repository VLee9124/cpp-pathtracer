#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// Common Headers
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

// C++ std
using std::make_shared;
using std::shared_ptr;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utils
inline double degrees_to_rad(double degrees)
{
  return degrees * pi / 180.0;
}