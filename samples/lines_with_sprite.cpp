#include <iostream>

#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Sprite* spr;

  Application app({
    .size = vu2d(500, 500), 
    .name = "Lines", 
    .clear_buffer = false,
    .on_launch = fn([&](Application& app) {
      spr = new Sprite("samples/test.png");
      app.RegisterSprite(spr);

      return pixel::ok;
    }),
    .on_update = fn([&](Application& app) {
      for (uint8_t i = 0; i < 50; i++) {
        app.DrawLine(
	  			vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
	  			vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
	  			RandPixel()
        );
      }

      app.DrawPartialSprite(app.MousePos(), vu2d(16, 16), vu2d(16, 16), spr);
      //app.DrawSprite(app.MousePos(), spr);

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    }),
    .on_close = fn([&](Application& app) {
      delete spr;
      return pixel::ok;
    })
  });

  app.Launch();

  return 0;
}
