#include <iostream>

#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Application app({
    .size = vu2d(500, 500), 
    .name = "Lines", 
    .clear_buffer = false,
    .on_launch = callback({
      app.CreateSprite("samples/test.png");

      return pixel::ok;
    }),
    .on_update = callback({
      for (uint8_t i = 0; i < 50; i++) {
        app.DrawLine(
	  			vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
	  			vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
	  			RandPixel()
        );
      }

      app.DrawPartialSprite(app.MousePos(), vu2d(16, 16), vu2d(16, 16), 0);
      //app.DrawSprite(app.MousePos(), 0);

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    })
  });

  app.Launch();

  return 0;
}
