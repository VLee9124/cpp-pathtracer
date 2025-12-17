#pragma once

#include "hittable.h"
#include "material.h"

class camera
{
public:
  double aspect_ratio = 1.0; // Width:Height
  int image_width = 100;
  int samples_per_pixel = 10;
  int max_iter = 10;

  void render(const hittable &world);

private:
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for a sum of pixel samples
  point3 camera_center;       // Camera center
  point3 pixel_00_location;   // Location of px 0,0
  vec3 pixel_delta_u;         // Right offset of pixel
  vec3 pixel_delta_v;         // Down offset of pixel

  void initialize();
  ray get_ray(int i, int j) const;
  color ray_color(const ray &r, int depth, const hittable &world) const;
  vec3 sample_square() const
  {
    // Returns vector to random point in [-.5, -.5] - [+.5, +.5] unit square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }
};

void camera::initialize()
{
  image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height; /* Ensure image height is at least 1px*/

  pixel_samples_scale = 1.0 / samples_per_pixel;

  auto camera_center = point3(0, 0, 0);

  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * (double(image_width) / image_height);

  // Calculate vectors across horizontal and vertical viewport axis
  auto viewport_u = vec3(viewport_width, 0, 0);
  auto viewport_v = vec3(0, -viewport_height, 0);

  // Calculate horizontal and vertical delta vectors between pixels ()
  pixel_delta_u = viewport_u / image_width;
  pixel_delta_v = viewport_v / image_height;

  /*
  Calculate location of upper pixel
  (1) subtract focal length to move onto the viewport plane
  (2) subtract half of view_u to move opposite of view_u axis (places you on left edge)
  (3) subtract half of view_v to move opposite of view_v axis (places you on top)
  */
  auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

  /*Half step from top corner to first pixel*/
  pixel_00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

void camera::render(const hittable &world)
{
  initialize();

  std::cout << "P3\n"
            << image_width << ' ' << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++)
  {
    std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    for (int i = 0; i < image_width; i++)
    {
      // Average sampled neighbors of i,j to get blur effect
      color pixel_color(0, 0, 0);
      for (int sample = 0; sample < samples_per_pixel; sample++)
      {
        ray r = get_ray(i, j);
        pixel_color += ray_color(r, max_iter, world);
      }
      write_color(std::cout, pixel_samples_scale * pixel_color);
    }
  }

  std::clog << "\rDone.               \n";
}

ray camera::get_ray(int i, int j) const
{
  // Construct a camera ray originating from the origin and directed at randomly sampled point around pixel location i, j
  auto offset = sample_square();
  auto pixel_sample = pixel_00_location + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

  auto ray_origin = camera_center;
  auto ray_direction = pixel_sample - ray_origin;

  return ray(ray_origin, ray_direction);
}

color camera::ray_color(const ray &r, int depth, const hittable &world) const
{
  if (depth <= 0)
    return color(0);

  hit_record rec;

  if (world.hit(r, interval(0.001, infinity), rec))
  {
    ray scattered;
    color attenuation;
    if (rec.mat->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, depth - 1, world);
    return color(0);
  }

  // Blue sky background
  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color(1.0) + a * color(0.5, 0.7, 1.0);
}