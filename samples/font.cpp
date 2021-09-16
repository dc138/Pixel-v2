#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Application app(
      {.size = vu2d(512, 512), .name = "Font", .on_update = fn([&](Application& app) {
                                                 app.DrawString({10, 10}, std::to_string(app.fps()));
                                                 app.DrawString(app.MousePos() + 10, to_string(app.MousePos()));

                                                 return app.Key(pixel::Key::KEY_A).pressed ? pixel::quit : pixel::ok;
                                               })});

  app.Launch();

  return 0;
}
