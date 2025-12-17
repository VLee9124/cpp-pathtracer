#pragma once

#include "utils.h"

class material;

class hit_record
{
public:
  point3 p;
  vec3 normal;
  shared_ptr<material> mat;
  double t;
  bool front_face;

  void set_face_normal(const ray &r, const vec3 &outward_normal)
  {
    // Sets hit record normal vector.
    // NOTE: 'outward normal' is assume to have unit length

    /*Compare ray and outward normal direction via dotprod.
    If dotprod is positive, they have same direction then the normal is inward. */
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable
{
public:
  virtual ~hittable() = default;
  virtual bool hit(const ray &r, interval ray_timeline, hit_record &rec) const = 0;
};