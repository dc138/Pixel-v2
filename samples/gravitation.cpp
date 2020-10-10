#include <iostream>

#include <pixel/pixel.hpp>
using namespace pixel;

int main() {
  Application app({.size = vu2d(500, 500), .name = "Lines",
  .on_launch = [] (Application& app) mutable -> pixel::rcode { // This function will be called when the application is created
    return pixel::ok;
  },
  .on_update = [] (Application& app) mutable -> pixel::rcode { // This function will be called every frame
    return pixel::ok;
  },
  .on_close = [] (Application& app) mutable -> pixel::rcode { // This function will be called when the application is about to close
    return pixel::ok;
  }});

  app.Launch();

  return 0;
}
