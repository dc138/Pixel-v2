#include <iostream>
#include <cmath>

#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  vf2d p1(250, 250);
  vf2d s1(0, 0);
  vf2d a1(0, 0);
  vf2d f1(0, 0);

  float m1 = 10000;
  float d1 = 0.005f;

  vf2d p2(350, 250);
  vf2d s2(0, 30);
  vf2d a2(0, 0);
  vf2d f2(0, 0);

  float m2 = 100;
  float d2 = 0.05f;

  float g = 10, s = 2;

  vf2d d(0, 0);
  float r = 0, f = 0, t = 0;

  Application app({
    .size = vu2d(500, 500),
    .name = "Gravitation",
    .on_update = callback_c({
      d = p1 - p2;

	  	r = sqrt((d.x * d.x) + (d.y * d.y));
	  	f = (g * m1 * m2) / (r * r);
	  	t = atan(d.y / d.x);

      f1 = vf2d(cos(t), sin(t)) * f * (p2.x >= p1.x ? 1 : -1);
      f2 = vf2d(cos(t), sin(t)) * f * (p2.x >= p1.x ? -1 : 1);

      a1 = f1 / m1;
      a2 = f2 / m2;

      s1 += (a1 * app.et() * s);
      s2 += (a2 * app.et() * s);

      p1 += (s1 * app.et() * s);
      p2 += (s2 * app.et() * s);

      app.FillCircle(p1, m1 * d1, White);
      app.FillCircle(p2, m2 * d2, White);

      app.DrawLine(p1, p1 + s1, Red);
	  	app.DrawLine(p2, p2 + s2, Red);

	  	app.DrawLine(p1, p1 + a1, Blue);
	  	app.DrawLine(p2, p2 + a2, Blue);

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    })
  });

  app.Launch();

  return 0;
}
