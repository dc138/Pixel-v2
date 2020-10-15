#include <iostream>
#include <cmath>

#include <pixel/pixel.hpp>
using namespace pixel;

#define px1 app.get<0>().at(0)
#define py1 app.get<0>().at(1)
#define sx1 app.get<0>().at(2)
#define sy1 app.get<0>().at(3) 
#define ax1 app.get<0>().at(4)
#define ay1 app.get<0>().at(5)
#define fx1 app.get<0>().at(6)
#define fy1 app.get<0>().at(7)
#define m1 app.get<0>().at(8)
#define d1 app.get<0>().at(9)

#define px2 app.get<1>().at(0)
#define py2 app.get<1>().at(1)
#define sx2 app.get<1>().at(2)
#define sy2 app.get<1>().at(3) 
#define ax2 app.get<1>().at(4)
#define ay2 app.get<1>().at(5)
#define fx2 app.get<1>().at(6)
#define fy2 app.get<1>().at(7)
#define m2 app.get<1>().at(8)
#define d2 app.get<1>().at(9)

#define g app.get<2>().at(0)
#define s app.get<2>().at(1)

#define dx app.get<2>().at(2)
#define dy app.get<2>().at(3)

#define r app.get<2>().at(4)
#define f app.get<2>().at(5)
#define t app.get<2>().at(6)

int main() {
  Application<std::array<float, 10>, std::array<float, 10>, std::array<float, 8>> 
  app({.size = vu2d(500, 500), .name = "Gravitation",
  .on_launch = [] (decltype(app)& app) mutable -> pixel::rcode { // This function will be called when the application is created
    px1 = 250; py1 = 250;
	  sx1 = 0; sy1 = 0;
	  ax1 = 0; ay1 = 0;
	  fx1 = 0; fy1 = 0;
	  m1 = 10000; d1 = 0.005f;

	  px2 = 350; py2 = 250;
	  sx2 = 0; sy2 = -30;
	  ax2 = 0; ay2 = 0;
	  fx2 = 0; fy2 = 0;
	  m2 = 100; d2 = 0.05f;

	  g = 10; s = 2;

	  dx = 0; dy = 0;
	  r = 0; f = 0, t = 0;

    return pixel::ok;
  },
  .on_update = [] (decltype(app)& app) mutable -> pixel::rcode { // This function will be called every frame
    app.Clear();

    dx = px1 - px2;
		dy = py1 - py2;

		r = sqrt((dx * dx) + (dy * dy));
		f = (g * m1 * m2) / (r * r);
		t = atan(dy / dx);

		fx1 = f * cos(t) * (px2 >= px1 ? 1 : -1);
		fy1 = f * sin(t) * (px2 >= px1 ? 1 : -1);
		fx2 = f * cos(t) * (px2 >= px1 ? -1 : 1);
		fy2 = f * sin(t) * (px2 >= px1 ? -1 : 1);

		ax1 = fx1 / m1;
		ay1 = fy1 / m1;
		ax2 = fx2 / m2;
		ay2 = fy2 / m2;

		sx1 += ax1 * app.et() * s;
		sy1 += ay1 * app.et() * s;
		sx2 += ax2 * app.et() * s;
		sy2 += ay2 * app.et() * s;

		px1 += sx1 * app.et() * s;
		py1 += sy1 * app.et() * s;
		px2 += sx2 * app.et() * s;
		py2 += sy2 * app.et() * s;

    app.FillCircle(vu2d(px1, py1), m1 * d1, White);
		app.FillCircle(vu2d(px2, py2), m2 * d2, White);

		app.DrawLine(vu2d(px1, py1), vu2d(px1 + sx1, py1 + sy1), Red);
		app.DrawLine(vu2d(px2, py2), vu2d(px2 + sx2, py2 + sy2), Red);

		app.DrawLine(vu2d(px1, py1), vu2d(px1 + ax1, py1 + ay1), Blue);
		app.DrawLine(vu2d(px2, py2), vu2d(px2 + ax2, py2 + ay2), Blue);

    if(app.KeyboardKey(Key::ESCAPE).pressed) {
      app.Close();
    }

    return pixel::ok;
  },
  .on_close = [] (decltype(app)& app) mutable -> pixel::rcode { // This function will be called when the application is about to close
    return pixel::ok;
  }});

  app.Launch();

  return 0;
}
