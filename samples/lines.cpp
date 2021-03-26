#include <iostream>

#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Application app({
    .size = vu2d(512, 512), 
    .name = "Lines", 
    .clear_buffer = false,
    .on_update = [](Application& app){
      for (uint8_t i = 0; i < 50; i++) {
        app.DrawLine(
          vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
	  			vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
	  			RandPixel()
        );
      }

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    }
  });

  app.Launch();

  return 0;
}
