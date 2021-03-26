#include <iostream>

#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Application app1({
    .size = vu2d(500, 500),
    .name = "Application One",
    .clear_buffer = false,
    .on_update = fn([&](Application& app){
      for (uint8_t i = 0; i < 50; i++) {
        app.DrawLine(
          vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
          vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
          RandPixel()
        );
      }

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    })
  });

  Application app2({
    .size = vu2d(500, 500),
    .name = "Application Two",
    .clear_buffer = false,
    .on_update = fn([&](Application& app){
      for (uint8_t i = 0; i < 50; i++) {
        app.DrawLine(
          vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
          vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
          RandPixel()
        );
      }

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    })
  });

  app1.Launch(true);
  app2.Launch(true);


  app1.EnsureClosed();
  app2.EnsureClosed();

  return 0;
}
