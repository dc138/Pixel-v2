#include <iostream>
#include <cmath>
#include <list>
#include <iterator>

#include <pixel/pixel.hpp>
using namespace pixel;

struct Particle {
  vf2d p = {0, 0};   
  vf2d s = {0, 0};   
  vf2d a = {0, 0};
  vf2d f = {0, 0};
  float m = 0;  
  float d = 0;
};

struct System {
  std::vector<Particle> particles;

  void Update(float et) {
    for (auto& p1 : particles) {
      for (auto& p2 : particles) {
        if (&p1 != &p2) {
          d = p1.p - p2.p;

	  	    r = sqrt((d.x * d.x) + (d.y * d.y));
	  	    f = (g * p1.m * p2.m) / (r * r);
	  	    t = atan(d.y / d.x);

          p1.f += vf2d(cos(t), sin(t)) * f * (p2.p.x >= p1.p.x ? 1 : -1);
          p2.f += vf2d(cos(t), sin(t)) * f * (p2.p.x >= p1.p.x ? -1 : 1);
        }
      }
    }

    for (auto& p : particles) {
      p.a = p.f / p.m;
      p.s += (p.a * et * s);
      p.p += (p.s * et * s);
      p.f = {0, 0};
    }
  }

  float g = 0, s = 0;
  float r = 0, f = 0, t = 0;
  vf2d d;
};

int main() {
  System sys {
    .g = 5,
    .s = 2
  };

  sys.particles.push_back({
    .p = {250, 250},
    .s = {0, 0},
    .a = {0, 0},
    .f = {0, 0},
    .m = 10000,
    .d = 0.005f
  });

  sys.particles.push_back({
    .p = {350, 250},
    .s = {0, 30},
    .a = {0, 0},
    .f = {0, 0},
    .m = 100,
    .d = 0.05f
  });

  sys.particles.push_back({
    .p = {150, 250},
    .s = {0, -30},
    .a = {0, 0},
    .f = {0, 0},
    .m = 100,
    .d = 0.05f
  });

  sys.particles.push_back({
    .p = {450, 250},
    .s = {0, 20},
    .a = {0, 0},
    .f = {0, 0},
    .m = 200,
    .d = 0.05f
  });

  struct Trace {
    vf2d p;
    float t;
  };

  std::vector<Trace> traces;
  
  bool show_pos = false;
  bool show_vel = false;
  bool show_acc = false;

  Application app({
    .size = vu2d(512, 512),
    .name = "Gravitation",
    .on_update = fn([&](Application& app) {
      sys.Update(app.et());

      for (decltype(traces)::iterator i = traces.begin(); i != traces.end(); ++i) {
        i->t += app.et();

        if (i->t > 2.0f) {
          traces.erase(i); 
          continue;
        }

        app.Draw(i->p, Pixel(0, 255, 255, 255 - (uint8_t)(i->t / 2.0f * 255.0f)));
      }

      for (auto& p : sys.particles) {
        app.FillCircle(p.p, p.m * p.d, White);
        app.DrawLine(p.p, p.p + p.s, Red);
	  	  app.DrawLine(p.p, p.p + p.a, Blue);

        traces.push_back({p.p, 0});

        if (show_pos) app.DrawString(p.p + 10, to_string(p.p), 8, Grey);
        if (show_vel) app.DrawString(p.p + 10, to_string(p.s), 8, Grey);
        if (show_acc) app.DrawString(p.p + 10, to_string(p.a), 8, Grey);
      }

      show_pos = app.Key(Key::P).pressed ? !show_pos : show_pos;
      show_vel = app.Key(Key::V).pressed ? !show_vel : show_vel;
      show_acc = app.Key(Key::A).pressed ? !show_acc : show_acc;

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    })
  });

  app.Launch();

  return 0;
}
