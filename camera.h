#pragma once

#include "hittable.h"
#include "material.h"

#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

#include <omp.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

class camera
{
public:
  double aspect_ratio   = 1.0;
  int image_width       = 100;
  int samples_per_pixel = 10;
  int max_iter          = 10;

  double vfov = 90;
  point3 lookfrom = point3(0);
  point3 lookat   = point3(0, 0, -1);
  vec3 vup        = vec3(0,1,0);

  double defocus_angle = 0;   // Variation angle of rays through each pixel
  double focus_dist = 10;     // Distance from camera lookfrom point to plane of perfect focus

  void render(const hittable &world);

private:
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for a sum of pixel samples
  point3 camera_center;       // Camera center
  point3 pixel_00_location;   // Location of px 0,0
  vec3 pixel_delta_u;         // Right offset of pixel
  vec3 pixel_delta_v;         // Down offset of pixel
  vec3 u, v, w;               // Camera frame basis vectors
  vec3 defocus_disk_u;
  vec3 defocus_disk_v;

  void initialize();
  ray get_ray(int i, int j, std::mt19937& ray) const;

  color ray_color(const ray &r, int depth, const hittable &world, std::mt19937& ray) const;

  vec3 sample_square(std::mt19937& rng) const {
    return vec3(random_double(rng) - 0.5, random_double(rng) - 0.5, 0);
  }
  
  point3 defocus_disk_sample(std::mt19937& rng) const {
    auto p = random_in_unit_disk(rng);
    return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }
};

void camera::initialize()
{
  image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height; /* Ensure image height is at least 1px*/

  pixel_samples_scale = 1.0 / samples_per_pixel;

  camera_center = lookfrom;


  // Determine viewport dim
  auto theta = degrees_to_rad(vfov);
  auto h = std::tan(theta/2);
  auto viewport_height = 2 * h * focus_dist;
  auto viewport_width = viewport_height * (double(image_width) / image_height);

  // Calculate u, v, w unit basis vectors for camera coordinate frame
  w = unit_vector(lookfrom - lookat);
  u = unit_vector(cross(vup, w));
  v = cross(w, u);

  // Calculate vectors across horizontal and vertical viewport axis
  auto viewport_u = viewport_width * u;     // Vector across viewport horizontal edge
  auto viewport_v = viewport_height * -v;   // Vector down viewport vertical edge

  // Calculate horizontal and vertical delta vectors between pixels ()
  pixel_delta_u = viewport_u / image_width;
  pixel_delta_v = viewport_v / image_height;

  // Calculate location of upper left pixel
  auto viewport_upper_left = camera_center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
  pixel_00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  // Calculate the camera defocus disk basis vectors
  auto defocus_radius = focus_dist * std::tan(degrees_to_rad(defocus_angle / 2));
  defocus_disk_u = u * defocus_radius;
  defocus_disk_v = v * defocus_radius;
}

void camera::render(const hittable &world)
{

  initialize();

  auto t1 = high_resolution_clock::now();

  std::vector<color> frameBuffer(image_width * image_height);
  
  std::atomic<int> completed{0};

  int num_threads = omp_get_max_threads();
  std::clog << "Using " << num_threads << " threads (OpenMP).\n";

  #pragma omp parallel 
  {
    int thread_id = omp_get_thread_num();

    std::seed_seq seq{1337u, (std::uint32_t)thread_id};
    std::mt19937 rng(seq);

    #pragma omp for schedule(dynamic, 1)
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; i++) {
        color pixel_color(0);

        for (int sample = 0; sample < samples_per_pixel; sample++){
          ray r = get_ray(i, j, rng);
          pixel_color += ray_color(r, max_iter, world, rng);
        }

        frameBuffer[j * image_width + i] = pixel_samples_scale * pixel_color;
      }

      int done = completed.fetch_add(1) + 1;

      #pragma omp critical 
      {
        std::clog << "\rScanlines remaining: " << (image_height - done) << ' ' << std::flush;
      }
    }
  }

  // Printing
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++){
    for (int i = 0; i < image_width; i++){
      write_color(std::cout, frameBuffer[j* image_width + i]);
    }
  }

  std::clog << "\rDone.               \n";

  auto t2 = high_resolution_clock::now();
  /* Getting number of milliseconds as an integer. */
  auto ms_int = duration_cast<milliseconds>(t2 - t1);

  /* Getting number of milliseconds as a double. */
  duration<double, std::milli> ms_double = t2 - t1;

  std::clog << ms_int.count() << "ms\n";
  std::clog << ms_double.count() << "ms\n";

}

ray camera::get_ray(int i, int j, std::mt19937& rng) const
{
  /* THREAD SAFE */
  // Construct a camera ray originating from the defocus disk and directed at randomly sampled point around pixel location i, j
  auto offset = sample_square(rng);
  auto pixel_sample = pixel_00_location + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

  auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample(rng);
  auto ray_direction = pixel_sample - ray_origin;

  return ray(ray_origin, ray_direction);
}

color camera::ray_color(const ray &r, int depth, const hittable &world, std::mt19937& rng) const
{
  if (depth <= 0)
    return color(0);

  hit_record rec;

  if (world.hit(r, interval(0.001, infinity), rec))
  {
    ray scattered;
    color attenuation;
    if (rec.mat->scatter(r, rec, attenuation, scattered, rng))
      return attenuation * ray_color(scattered, depth - 1, world, rng);
    return color(0);
  }

  // Blue sky background
  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color(1.0) + a * color(0.5, 0.7, 1.0);
}