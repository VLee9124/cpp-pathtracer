#pragma once

#include <limits>

// NOTE: infinity is already declared in utils.h but compiler complains, so redefine inf

const double inf = std::numeric_limits<double>::infinity();

class interval
{
public:
  double min;
  double max;
  static const interval empty;
  static const interval universe;

  interval() : min(+inf), max(-inf) {}
  interval(double min, double max) : min(min), max(max) {}

  double size() const
  {
    return max - min;
  }

  bool contains(double x) const
  {
    return min <= x && x <= max;
  }

  bool surrounds(double x) const
  {
    return min < x && x < max;
  }
};

const interval interval::empty = interval(+inf, -inf);
const interval interval::universe = interval(-inf, +inf);