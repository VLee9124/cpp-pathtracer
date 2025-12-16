#include "color.h"
#include "vec3.h"
#include "ray.h"

#include <iostream>

color ray_color(const ray &r)
{
  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);

  // return color(1.0, 1.0, 1.0) * unit_direction;
}

int main()
{
  // Image

  auto aspect_ratio = 16.0 / 9.0; // Width:Height
  int image_width = 400;

  int image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height; /* Ensure image height is at least 1px*/

  // Camera

  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * (double(image_width) / image_height);
  auto camera_center = point3(0, 0, 0);

  // Calculate vectors across horizontal and vertical viewport axis
  auto viewport_u = vec3(viewport_width, 0, 0);
  auto viewport_v = vec3(0, -viewport_height, 0);

  // Calculate horizontal and vertical delta vectors between pixels ()
  auto pixel_delta_u = viewport_u / image_width;
  auto pixel_delta_v = viewport_v / image_height;

  // Calculate location of upper pixel
  auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2; /*(1) subtract focal length to move onto the viewport plane (2) subtract half of view_u to move opposite of view_u axis (places you on left edge) (3) subtract half of view_v to move opposite of view_v axis (places you on top)*/
  auto pixel_00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);                  /*Half step from top corner to first pixel*/

  // Rendering output

  std::cout << "P3\n"
            << image_width << ' ' << image_height << "\n255\n";

  for (int i = 0; i < image_height; i++)
  {
    std::clog << "\rScanlines remaining: " << (image_height - i) << ' ' << std::flush;
    for (int j = 0; j < image_width; j++)
    {
      auto pixel_center = pixel_00_location + (j * pixel_delta_u) + (i * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      ray r(camera_center, ray_direction);

      color pixel_color = ray_color(r);
      write_color(std::cout, pixel_color);
    }
  }
  std::clog << "\rDone.               \n";
}
