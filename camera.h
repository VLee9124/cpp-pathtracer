#pragma once

#include "hittable.h"

class camera
{
public:
  double aspect_ratio = 1.0; // Width:Height
  int image_width = 100;

  void render(const hittable &world)
  {
    initialize();

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

        color pixel_color = ray_color(r, world);
        write_color(std::cout, pixel_color);
      }
    }
    std::clog << "\rDone.               \n";
  }

private:
  int image_height;         // Rendered image height
  point3 camera_center;     // Camera center
  point3 pixel_00_location; // Location of px 0,0
  vec3 pixel_delta_u;       // Right offset of pixel
  vec3 pixel_delta_v;       // Down offset of pixel

  void initialize()
  {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height; /* Ensure image height is at least 1px*/

    // Camera
    auto camera_center = point3(0, 0, 0);

    // Determine viewport dim
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    // Calculate vectors across horizontal and vertical viewport axis
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate horizontal and vertical delta vectors between pixels ()
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate location of upper pixel
    /*
    (1) subtract focal length to move onto the viewport plane
    (2) subtract half of view_u to move opposite of view_u axis (places you on left edge) (3) subtract half of view_v to move opposite of view_v axis (places you on top)
    */
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

    /*Half step from top corner to first pixel*/
    pixel_00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  color ray_color(const ray &r, const hittable &world) const
  {
    hit_record rec;

    if (world.hit(r, interval(0, infinity), rec))
    {
      return 0.5 * (rec.normal + color(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0) + a * color(0.5, 0.7, 1.0);
  }
};