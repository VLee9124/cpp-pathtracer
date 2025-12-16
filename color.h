#pragma once

#include <vec3.h>

#include <iostream>

using color = vec3;

void write_color(std::ostream &out, const color &pixel_color)
{
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Map [0,1] range to [0,255] range

  int rbyte = int(255.999 * r);
  int gbyte = int(255.999 * g);
  int bbyte = int(255.999 * b);

  // Output pixel color components
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}