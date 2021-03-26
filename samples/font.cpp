#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Application app({
    .size = vu2d(512, 512), 
    .name = "Font",
    .on_update = fn([&](Application& app){
      app.DrawSprite(vu2d(0, 0), app.pFontSprite, vf2d(0.5f, 0.5f));

      return app.Key(Key::ESCAPE).pressed ? pixel::quit : pixel::ok;
    })
  });

  app.Launch();

  return 0;
}