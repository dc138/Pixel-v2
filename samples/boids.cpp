#include <pixel/pixel.hpp>
using namespace pixel;

static Application *global_app;

struct Boid {
  vf2d pos = {0.f, 0.f};
  vf2d vel = {0.f, 0.f};

  vf2d acc = {0.f, 0.f};
  float angle = 0.f;

  vf2d flock = {0.f, 0.f};
  vf2d flock_heading = {0.f, 0.f};
  uint16_t flockmates;
};

struct System {
  float min_speed = 0;
  float max_speed = 0;

  float view_dis = 0;
  float view_angle = 0;
  float local_sense = 0;

  float boid_avoidance_factor = 0;
  float flock_strenght = 0;

  float max_force = 0;

  std::vector<Boid> boids;

  void Update(float et, const vf2d &mouse, bool follow_mouse) {
    auto clamp_angle = [&](float &angle) {
      if (angle <= 0)
        angle += 2 * M_PI;
      if (angle >= 2 * M_PI)
        angle -= 2 * M_PI;
    };

    auto clamp_vector = [](vf2d vector, float max) {
      if (vector.x > max)
        vector.x = max;
      if (vector.y > max)
        vector.y = max;

      return vector;
    };

    for (Boid &b1 : boids) {
      b1.angle = atanf(b1.vel.y / b1.vel.x);
      b1.angle += (b1.vel.x < 0) ? M_PI : 0;

      clamp_angle(b1.angle);

      b1.acc = {0, 0};
      b1.flock = {0, 0};
      b1.flock_heading = {0, 0};
      b1.flockmates = 0;

      for (Boid &b2 : boids) {
        if (&b1 == &b2)
          continue;

        float dm = (b1.pos - b2.pos).mod();
        vf2d d = b1.pos - b2.pos;

        if (dm > view_dis)
          continue;

        float a = atanf(d.y / d.x);
        a += (d.x < 0) ? 2 * M_PI : M_PI;
        clamp_angle(a);

        float b = b1.angle - a;
        clamp_angle(b);

        if (!(b > (2 * M_PI - view_angle) || b < view_angle ||
              dm < local_sense))
          continue;

        b1.flock += b2.pos;
        b1.flock_heading += b2.vel;
        b1.flockmates++;

        float g = (b > M_PI) ? (2 * M_PI - b) : b;
        g /= view_angle;

        b1.acc += (d * boid_avoidance_factor / (dm * (g + .01f)));
      }

      b1.flock /= b1.flockmates;
      b1.flock_heading /= b1.flockmates;

      vf2d d = b1.flock - b1.pos;

      b1.acc += clamp_vector(d * flock_strenght, max_force);
      b1.acc += clamp_vector(b1.flock_heading * flock_strenght, max_force);
      if (follow_mouse)
        b1.acc += clamp_vector(mouse - b1.pos, max_force);

      b1.vel += (b1.acc * et);

      float speed = b1.vel.mod();
      vf2d vel_norm = b1.vel / speed;

      speed = std::clamp(speed, min_speed, max_speed);
      speed = (speed + (max_speed - min_speed)) / 2;

      b1.vel = vel_norm * speed;

      b1.pos += (b1.vel * et);

      if (b1.pos.x <= .0f)
        b1.pos.x += 1024.f;
      if (b1.pos.y <= .0f)
        b1.pos.y += 1024.f;
      if (b1.pos.x >= 1024.f)
        b1.pos.x -= 1024.f;
      if (b1.pos.y >= 1024.f)
        b1.pos.y -= 1024.f;
    }
  }
};

int main() {
  System sys{.min_speed = 20,
             .max_speed = 85,
             .view_dis = 115,
             .view_angle = .75f * M_PI,
             .local_sense = 25.f,
             .boid_avoidance_factor = 40.f,
             .flock_strenght = .25f,
             .max_force = 15.f};

  srand(time(0));
  auto rand_float = [](float min, float max) {
    return (float)rand() / (float)RAND_MAX * (max - min) + min;
  };

  for (uint8_t n = 0; n < 100; n++) {
    sys.boids.push_back({{rand_float(0.f, 1024.f), rand_float(0.f, 1024.f)},
                         {rand_float(-sys.max_speed, sys.max_speed),
                          rand_float(-sys.max_speed, sys.max_speed)}});
  }

  bool run = true;
  bool mouse = false;

  Application app(
      {.size = vu2d(1024, 1024),
       .name = "Boids",
       .on_update = fn([&](Application &app) {
         sys.Update(run ? app.et() : 0, app.MousePos(), mouse);

         for (const Boid &boid : sys.boids) {
           vf2d pos1 = boid.pos + (vf2d(cosf(boid.angle + (0 * M_PI)),
                                        sinf(boid.angle + (0 * M_PI))) *
                                   5.0f);
           vf2d pos2 = boid.pos + (vf2d(cosf(boid.angle + (0.75f * M_PI)),
                                        sinf(boid.angle + (0.75f * M_PI))) *
                                   6.0f);
           vf2d pos3 = boid.pos + (vf2d(cosf(boid.angle + (-0.75f * M_PI)),
                                        sinf(boid.angle + (-0.75f * M_PI))) *
                                   6.0f);

           app.DrawLine(pos1, pos2, Pixel(255, 94, 94));
           app.DrawLine(pos1, pos3, Pixel(255, 94, 94));

           app.DrawLine(boid.pos, pos2, Pixel(255, 94, 94));
           app.DrawLine(boid.pos, pos3, Pixel(255, 94, 94));
         }

         run = app.Key(Key::R).pressed ? !run : run;
         mouse = app.Key(Key::M).pressed ? !mouse : mouse;

         if (!run)
           app.DrawString({10, 10}, "Paused");
         if (mouse && run)
           app.DrawString({10, 10}, "Following mouse", 32);

         return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
       })});

  global_app = &app;
  app.Launch();

  return 0;
}
