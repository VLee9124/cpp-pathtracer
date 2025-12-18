#pragma once

#include <cmath>
#include <cstdlib>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

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

inline double random_double()
{
  // Return real val in [0,1]
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

inline double random_double(double min, double max)
{
  // Return real val in [min, max]
  return min + (max - min) * random_double();
}

// Thread safe versions for parallel computation
inline double random_double(std::mt19937& rng){
  std::uniform_real_distribution<double> distribution(0.0, 1.0);
  return distribution(rng);
}

inline double random_double(double min, double max, std::mt19937& rng)
{
  // Return real val in [min, max]
  return min + (max - min) * random_double(rng);
}

// Common Headers
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"