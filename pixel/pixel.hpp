/*
              _________________________

                The Pixel Library
              _________________________


  ABOUT:
  ~~~~~

  A single file object oriented library to more easily create fast graphical
  c++ applications that abstracts the  creation of windows and makes using
  opengl much easier.

  It is intended to help newer programmers more easily make their first graphical
  application to learn the basics of 2d graphics, and later 3d graphics.

  Only linux build are supported at the moment, as this library makes use of
  X11. Windows and MacOS are coming in the future.


  LICENSE:
  ~~~~~~~

  MIT License

  Copyright (c) 2020 Antonio de Haro

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.


  SAMPLE PROGRAM:
  ~~~~~~~~~~~~~~~

  The folowing program will create a simple application window and draw 50 random
  lines (color and position) every frame.

  #include <iostream>
  #include <pixel/pixel.hpp>
  using namespace pixel;

  int main() {
    Application app({.size = vu2d(500, 500), .name = "Lines",
    .on_update = [] (Application& app) mutable -> pixel::rcode {
    for (uint8_t i = 0; i < 50; i++) {
      app.DrawLine(
      vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
        vu2d(rand() % app.ScreenSize().x, rand() % app.ScreenSize().y),
        RandPixel()
      );
    }

    if(app.Key(Key::ESCAPE).pressed) {
      app.Close();
    }

    return pixel::ok;
    }});

    app.Launch();

    return 0;
  }

  COMPILING:
  ~~~~~~~~~

  Only linux systems are supported at this moment, but support for MacOS and windows
  will come in the future. To compile under linux you will need a modern C++ compiler,
  as this library makes use of C++20 features, so update your compiler if you haven't
  already. This library has only been tested with GCC, so CLANG builds might not work
  correctly. To compile simply run:

     g++ -o YourProgramName YourProgramName.cpp -lX11 -lGL -lpthread -lpng -std=c++20 -O2 \
     -I. -Wall -pedantic-errors && ./YourProgramName


  AUTHOR
  ~~~~~~

  Antonio de Haro

*/

#pragma once

#ifdef __linux__
#  define PIXEL_LINUX
#elif defined(_WIN32)
#  define PIXEL_WIN
#elif defined(__APPLE__)
#  define PIXEL_MACOS
#else
#  error "Unsupported platform."
#endif

#define PIXEL_VERSION_MAJOR 3
#define PIXEL_VERSION_MINOR 0
#define PIXEL_VERSION_PATCH 0

#define PIXEL_VERSION (PIXEL_VERSION_MAJOR * 1000000) + (PIXEL_VERSION_MINOR * 1000) + (PIXEL_VERSION_PATCH)

#define IGNORE(x) (void(x))

#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <istream>
#include <map>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

/*
___________________________

  Header declarations
___________________________

*/

#ifdef PIXEL_USE_DIRECTX
#  error "Currently unsupported renderer."
#endif /* PIXEL_USE_DIRECTX */

#include <GLFW/glfw3.h>

namespace pixel {
  enum rcode { ok = 0, err = 1, file_err = 2, abort = 3, quit = 4 };
  class Application;
  class Sprite;

  struct Button {
    bool pressed  = false;
    bool held     = false;
    bool released = false;
  };

  enum class Key : uint8_t {
    NONE,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    K0,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    K8,
    K9,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE,
    TAB,
    SHIFT,
    CTRL,
    INS,
    DEL,
    HOME,
    END,
    PGUP,
    PGDN,
    BACK,
    ESCAPE,
    RETURN,
    ENTER,
    PAUSE,
    SCROLL,
    NP0,
    NP1,
    NP2,
    NP3,
    NP4,
    NP5,
    NP6,
    NP7,
    NP8,
    NP9,
    NP_MUL,
    NP_DIV,
    NP_ADD,
    NP_SUB,
    NP_DECIMAL,
    PERIOD
  };

  enum class Mouse : uint8_t { LEFT = 0, MIDDLE = 1, RIGHT = 2 };

  enum class DrawingMode : uint8_t { NO_ALPHA, FULL_ALPHA, MASK };

  template <class T>
  struct v2d {
    T x = 0;
    T y = 0;

    v2d() : x(0), y(0) {}
    v2d(T x, T y) : x(x), y(y) {}
    v2d(const v2d& v) : x(v.x), y(v.y) {}

    T prod() const noexcept { return x * y; }
    T mod() const noexcept { return sqrt((x * x) + (y * y)); }

    v2d abs() const noexcept { return v2d(std::abs(x), std::abs(y)); }

    v2d operator+(const T& rhs) const noexcept { return v2d(this->x + rhs, this->y + rhs); }
    v2d operator-(const T& rhs) const noexcept { return v2d(this->x - rhs, this->y - rhs); }
    v2d operator*(const T& rhs) const noexcept { return v2d(this->x * rhs, this->y * rhs); }
    v2d operator/(const T& rhs) const noexcept { return v2d(this->x / rhs, this->y / rhs); }
    v2d operator%(const T& rhs) const noexcept { return v2d(this->x % rhs, this->y % rhs); }

    v2d operator+(const v2d& rhs) const noexcept { return v2d(this->x + rhs.x, this->y + rhs.y); }
    v2d operator-(const v2d& rhs) const noexcept { return v2d(this->x - rhs.x, this->y - rhs.y); }
    v2d operator*(const v2d& rhs) const noexcept { return v2d(this->x * rhs.x, this->y * rhs.y); }
    v2d operator/(const v2d& rhs) const noexcept { return v2d(this->x / rhs.x, this->y / rhs.y); }
    v2d operator%(const v2d& rhs) const noexcept { return v2d(this->x % rhs.x, this->y % rhs.y); }

    v2d& operator+=(const T& rhs) noexcept {
      this->x += rhs;
      this->y += rhs;
      return *this;
    }
    v2d& operator-=(const T& rhs) noexcept {
      this->x -= rhs;
      this->y -= rhs;
      return *this;
    }
    v2d& operator*=(const T& rhs) noexcept {
      this->x *= rhs;
      this->y *= rhs;
      return *this;
    }
    v2d& operator/=(const T& rhs) noexcept {
      this->x /= rhs;
      this->y /= rhs;
      return *this;
    }

    v2d& operator+=(const v2d& rhs) noexcept {
      this->x += rhs.x;
      this->y += rhs.y;
      return *this;
    }
    v2d& operator-=(const v2d& rhs) noexcept {
      this->x -= rhs.x;
      this->y -= rhs.y;
      return *this;
    }
    v2d& operator*=(const v2d& rhs) noexcept {
      this->x *= rhs.x;
      this->y *= rhs.y;
      return *this;
    }
    v2d& operator/=(const v2d& rhs) noexcept {
      this->x /= rhs.x;
      this->y /= rhs.y;
      return *this;
    }

    template <typename U>
    operator v2d<U>() const noexcept {
      return {static_cast<U>(this->x), static_cast<U>(this->y)};
    }
  };

  template <typename T>
  v2d<T> operator+(const T& lhs, const v2d<T>& rhs) {
    return v2d<T>(lhs + rhs.x, lhs + rhs.y);
  }
  template <typename T>
  v2d<T> operator-(const T& lhs, const v2d<T>& rhs) {
    return v2d<T>(lhs - rhs.x, lhs - rhs.y);
  }
  template <typename T>
  v2d<T> operator*(const T& lhs, const v2d<T>& rhs) {
    return v2d<T>(lhs * rhs.x, lhs * rhs.y);
  }
  template <typename T>
  v2d<T> operator/(const T& lhs, const v2d<T>& rhs) {
    return v2d<T>(lhs / rhs.x, lhs / rhs.y);
  }
  template <typename T>
  v2d<T> operator%(const T& lhs, const v2d<T>& rhs) {
    return v2d<T>(lhs % rhs.x, lhs % rhs.y);
  }

  typedef v2d<int32_t>  vi2d;
  typedef v2d<uint32_t> vu2d;
  typedef v2d<uint16_t> vs2d;
  typedef v2d<uint8_t>  vc2d;
  typedef v2d<double>   vd2d;
  typedef v2d<float>    vf2d;

  template <typename T>
  std::string to_string(const v2d<T>& vector) {
    return std::to_string(vector.x) + ", " + std::to_string(vector.y);
  }

  struct Pixel {
    union {
      uint32_t n = 0x000000FF;
      struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
      } v;
    };

    Pixel();
    Pixel(uint32_t p);
    Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);

    bool operator==(const Pixel& p) const;
    bool operator!=(const Pixel& p) const;
  };

  const Pixel Grey(192, 192, 192);
  const Pixel DarkGrey(128, 128, 128);
  const Pixel VeryDarkGrey(64, 64, 64);

  const Pixel Red(255, 0, 0);
  const Pixel DarkRed(128, 0, 0);
  const Pixel VeryDarkRed(64, 0, 0);

  const Pixel Yellow(255, 255, 0);
  const Pixel DarkYellow(128, 128, 0);
  const Pixel VeryDarkYellow(64, 64, 0);

  const Pixel Green(0, 255, 0);
  const Pixel DarkGreen(0, 128, 0);
  const Pixel VeryDarkGreen(0, 64, 0);

  const Pixel Cyan(0, 255, 255);
  const Pixel DarkCyan(0, 128, 128);
  const Pixel VeryDarkCyan(0, 64, 64);

  const Pixel Blue(0, 0, 255);
  const Pixel DarkBlue(0, 0, 128);
  const Pixel VeryDarkBlue(0, 0, 64);

  const Pixel Magenta(255, 0, 255);
  const Pixel DarkMagenta(128, 0, 128);
  const Pixel VeryDarkMagenta(64, 0, 64);

  const Pixel White(255, 255, 255);
  const Pixel Black(0, 0, 0);
  const Pixel Blank(0, 0, 0, 0);

  Pixel RandPixel() { return Pixel(rand() % 255, rand() % 255, rand() % 255, rand() % 255); }

  class Sprite final {
   public:
    Sprite(const std::string& filename);
    Sprite(uint32_t w, uint32_t h);
    ~Sprite();

    friend class Application;
    friend class FileUtil;
    friend class Renderer;
    friend class Platform;

   public:
    Sprite(const Sprite& src);
    Sprite& operator=(const Sprite& rhs);

    Sprite(Sprite&& src) noexcept;
    Sprite& operator=(Sprite&& rhs) noexcept;

   public:
    Pixel GetPixel(uint32_t x, uint32_t y) const;
    void  SetPixel(uint32_t x, uint32_t y, const Pixel& p);

    vu2d GetSize() const;

   public:
    void Swap(Sprite& other) noexcept;

   private:
    vu2d pSize;
    vf2d pUvScale = vf2d(1.0f, 1.0f);

    Pixel*   pBuffer   = nullptr;
    uint32_t pBufferId = 0xFFFFFFFF;
  };

  struct SpriteRef {
    Sprite* pSprite;
    Pixel   pTint = White;

    vf2d  pPos[4] = {vf2d(0.0f, 0.0f), vf2d(0.0f, 0.0f), vf2d(0.0f, 0.0f), vf2d(0.0f, 0.0f)};
    vf2d  pUv[4]  = {vf2d(0.0f, 0.0f), vf2d(0.0f, 1.0f), vf2d(1.0f, 1.0f), vf2d(1.0f, 0.0f)};
    float pW[4]   = {1.0f, 1.0f, 1.0f, 1.0f};
  };

  class FileUtil final {
   public:
    static rcode LoadImage(Sprite* spr, const std::string& filename);
    static rcode SaveImage(Sprite* spr, const std::string& filename);
  };

  class Renderer final {
   public:
    friend class Platform;
    friend class Application;

    rcode CreateDevice(GLFWwindow* window, bool fullscreen, bool vsync);

    void DisplayFrame();
    void PrepareDrawing();
    void DrawLayerQuad();
    void DrawDecalQuad(const SpriteRef& sprite);

    uint32_t CreateTexture(uint32_t width, uint32_t height);
    uint32_t DeleteTexture(uint32_t id);
    void     UpdateTexture(uint32_t id, Sprite* spr);
    void     UpdateTexture(uint32_t id, uint32_t w, uint32_t h, Pixel* buffer);
    void     ApplyTexture(uint32_t id);

    void UpdateViewport(const vu2d& pos, const vu2d& size);
    void ClearBuffer(Pixel p, bool depth);

   private:
    Application* App;
    GLFWwindow*  pWindow;
  };

  class Platform final {
   public:
    friend class Renderer;
    friend class Application;

    void ApplicationStartUp();
    void ApplicationCleanUp();

    rcode CreateGraphics(bool fullscreen, bool vsync, const vu2d& viewpos, const vu2d& viewsize);
    rcode CreateWindowPane(const vu2d& winpos, vu2d& winsize, bool fullscreen);
    void  SetWindowTitle(const std::string& s);

    void StartSystemEventLoop();
    void HandleSystemEvent();

   private:
    Application* App;
    GLFWwindow*  pWindow;
  };

  static std::map<size_t, uint8_t> pKeyMap;

  class Application final {
   public:
    typedef rcode (*callback_t)(Application&);
    typedef struct params {
      vu2d    size     = vu2d(256, 256);
      vu2d    position = vu2d(25, 25);
      uint8_t scale    = 2;

      std::string        name = "Pixel";
      pixel::DrawingMode mode = DrawingMode::FULL_ALPHA;

      bool  fullscreen   = false;
      bool  vsync        = false;
      bool  clear_buffer = true;
      Pixel buffer_color = Black;

      callback_t on_launch = nullptr;
      callback_t on_update = nullptr;
      callback_t on_close  = nullptr;
    } params_t;

   public:
    Application(params_t params);
    ~Application();

    friend class Platform;
    friend class Sprite;

   public:
    rcode Launch(bool background = false);

   public:
    Application(const Application& other) = delete;
    Application& operator=(const Application& other) = delete;

   public:
    void Close();
    void EnsureClosed();

    void SetName(const std::string& name);
    void SetDrawingMode(pixel::DrawingMode mode);

   public:
    void RegisterSprite(Sprite* spr);

   public:
    void Draw(const vu2d& pos, const Pixel& pixel = White);
    void DrawLine(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel = White);
    void DrawString(const vu2d& pos, std::string_view text, uint8_t size = 8, const Pixel& color = White);

    void DrawCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel = White);
    void FillCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel = White);

    void DrawRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel = White);
    void FillRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel = White);

    void DrawTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel = White);
    void FillTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel = White);

    void DrawSprite(const vu2d& pos, Sprite* spr, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);
    void DrawPartialSprite(const vu2d&  pos,
                           const vu2d&  spos,
                           const vu2d&  ssize,
                           Sprite*      spr,
                           const vf2d&  scale = vf2d(1.0f, 1.0f),
                           const Pixel& tint  = White);

    void DrawWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& pos, const Pixel& tint = White);
    void DrawPartialWarpedSprite(uint8_t              sprite,
                                 std::array<vf2d, 4>& post,
                                 const vf2d&          spos,
                                 const vf2d&          ssize,
                                 const Pixel&         tint = White);

    void DrawRotatedSprite(const vf2d&  pos,
                           uint8_t      sprite,
                           float        alpha,
                           const vf2d&  center = vf2d(0.0f, 0.0f),
                           const vf2d   scale  = vf2d(1.0f, 1.0f),
                           const Pixel& tint   = White);
    void DrawPartialRotatedSprite(const vf2d&  pos,
                                  uint8_t      sprite,
                                  float        alpha,
                                  const vf2d&  spos,
                                  const vf2d&  ssize,
                                  const vf2d&  center = vf2d(0.0f, 0.0f),
                                  const vf2d   scale  = vf2d(1.0f, 1.0f),
                                  const Pixel& tint   = White);

   private:
    void UpdateMouse(uint32_t x, uint32_t y);
    void UpdateMouseWheel(uint32_t delta);
    void UpdateWindowSize(uint32_t x, uint32_t y);

    void UpdateViewport();

    void UpdateMouseState(uint32_t button, bool state);
    void UpdateKeyState(uint32_t key, bool state);

    void UpdateMouseFocus(bool state);
    void UpdateKeyFocus(bool state);

   public:
    pixel::DrawingMode DrawingMode() const;
    const vu2d&        ScreenSize() const;

    const vu2d& WindowSize() const;
    const vu2d& WindowPos() const;

    const vu2d& MousePos() const;
    uint32_t    MouseWheel() const;

    const Button& Mouse(pixel::Mouse button) const;
    const Button& Key(Key key) const;

    float    et() const;
    uint32_t fps() const;

   private:
    vu2d pWindowSize;
    vu2d pWindowPos;

    vu2d pScreenSize;
    vf2d pInvScreenSize;

    uint8_t pScale;

    vu2d pViewSize;
    vu2d pViewPos;

   private:
    std::string pWindowName   = "";
    std::string pWindowTittle = "";

   private:
    bool pVsync      = false;
    bool pFullScreen = false;

    bool  pClearBuffer = true;
    Pixel pBufferColor = Black;

    std::atomic<bool> pHasBeenClosed {false};
    std::atomic<bool> pThreadRunning {false};
    std::atomic<bool> pWantsToClose {false};

   private:
    vu2d     pMousePos;
    uint32_t pMouseWheel = 0;

    Button pMouseButtons[3]    = {};
    bool   pMouseButtonsOld[3] = {};
    bool   pMouseButtonsNew[3] = {};

    Button pKeyboardKeys[256]    = {};
    bool   pKeyboardKeysOld[256] = {};
    bool   pKeyboardKeysNew[256] = {};

    bool pHasInputFocus = false;
    bool pHasMouseFocus = false;

   private:
    std::chrono::system_clock::time_point pClock1;
    std::chrono::system_clock::time_point pClock2;
    std::chrono::duration<float>          pElapsedTimer;

    float    pElapsedTime = 0.0f;
    float    pFrameTimer  = 0.0f;
    uint32_t pFrameCount  = 0;
    uint32_t pFrameRate   = 0;

   private:
    Sprite*  pFontSprite;
    Pixel*   pBuffer   = nullptr;
    uint32_t pBufferId = 0xFFFFFFFF;

    std::vector<SpriteRef> pSpritesPending;
    pixel::DrawingMode     pDrawingMode = pixel::DrawingMode::NO_ALPHA;

   private:
    callback_t pOnLaunch;
    callback_t pOnUpdate;
    callback_t pOnClose;

    Platform pPlatform;
    Renderer pRenderer;

   private:
    void pStartThread();
    void pEngineThread();
    void pCreateFont();
  };
}

/*
____________________________

  Function definitions
____________________________

*/

namespace pixel {
  Pixel::Pixel() {
    v.r = 0;
    v.g = 0;
    v.b = 0;
    v.a = 0xFF;
  }
  Pixel::Pixel(uint32_t p) { n = p; }

  Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    n = red | (green << 8) | (blue << 16) | (alpha << 24);
  }

  bool Pixel::operator==(const Pixel& p) const { return n == p.n; }
  bool Pixel::operator!=(const Pixel& p) const { return n != p.n; }

  Sprite::Sprite(uint32_t w, uint32_t h) {
    if (pBuffer) delete[] pBuffer;

    pSize.x = w;
    pSize.y = h;

    pBuffer = new Pixel[w * h];
    for (uint32_t i = 0; i < w * h; i++) {
      pBuffer[i] = Pixel();
    }
  }

  Sprite::~Sprite() {
    if (pBuffer) delete[] pBuffer;
  }

  Sprite::Sprite(const std::string& filename) {
    if (FileUtil::LoadImage(this, filename) != rcode::ok)
      throw std::runtime_error(std::string("Cannot open: ") + filename);
  }

  Sprite::Sprite(const Sprite& src) : pSize(src.pSize), pUvScale(src.pUvScale), pBufferId(src.pBufferId) {
    pBuffer = new Pixel[src.pSize.prod()];

    for (uint32_t i = 0; i < src.pSize.prod(); i++) {
      pBuffer[i] = src.pBuffer[i];
    }
  }

  Sprite& Sprite::operator=(const Sprite& rhs) {
    Sprite copy(rhs);
    Swap(copy);
    return *this;
  }

  Sprite::Sprite(Sprite&& src) noexcept {
    pBuffer = nullptr;
    Swap(src);
  }

  Sprite& Sprite::operator=(Sprite&& rhs) noexcept {
    Swap(rhs);
    return *this;
  }

  Pixel Sprite::GetPixel(uint32_t x, uint32_t y) const {
    if (x < pSize.x && y < pSize.y) {
      return pBuffer[y * pSize.x + x];

    } else {
      return Pixel(0, 0, 0, 0);
    }
  }

  void Sprite::SetPixel(uint32_t x, uint32_t y, const Pixel& p) {
    if (x < pSize.x && y < pSize.y) {
      pBuffer[y * pSize.x + x] = p;
    }
  }

  vu2d Sprite::GetSize() const { return pSize; }

  void Sprite::Swap(Sprite& other) noexcept {
    std::swap(pSize, other.pSize);
    std::swap(pUvScale, other.pUvScale);

    std::swap(pBuffer, other.pBuffer);
    std::swap(pBufferId, other.pBufferId);
  }
}

namespace pixel {
  Application::Application(Application::params_t params) {
    if (params.scale <= 0 || params.size.x <= 0 || params.size.y <= 0)
      throw std::runtime_error("Invalid Application dimensions");

    pPlatform.App = this;
    pRenderer.App = this;

    pWindowSize = params.size * params.scale;
    pWindowPos  = params.position;

    pScreenSize    = params.size;
    pInvScreenSize = vf2d(1.0f / params.size.x, 1.0f / params.size.y);
    pScale         = params.scale;

    pWindowName   = params.name;
    pWindowTittle = params.name + " - FPS: 0";

    pDrawingMode = params.mode;

    pVsync       = params.vsync;
    pFullScreen  = params.fullscreen;
    pClearBuffer = params.clear_buffer;
    pBufferColor = params.buffer_color;

    pOnLaunch = params.on_launch;
    pOnUpdate = params.on_update;
    pOnClose  = params.on_close;

    pCreateFont();
  }

  Application::~Application() { delete pFontSprite; }

  void Application::pStartThread() {
    pPlatform.ApplicationStartUp();

    if (pPlatform.CreateWindowPane(pWindowPos, pWindowSize, pFullScreen) != rcode::ok) return;

    pThreadRunning = true;

    UpdateViewport();

    std::thread thread = std::thread(&pixel::Application::pEngineThread, this);
    thread.detach();

    pPlatform.StartSystemEventLoop();
    pPlatform.ApplicationCleanUp();
  }

  void Application::pEngineThread() {
    pPlatform.ThreadStartUp();

    if (pPlatform.CreateGraphics(pFullScreen, pVsync, pViewPos, pViewSize) == rcode::err) return;

    RegisterSprite(pFontSprite);

    pBuffer = new Pixel[pScreenSize.prod()];
    for (uint32_t i = 0; i < pScreenSize.prod(); i++) {
      pBuffer[i] = Pixel();
    }

    pBufferId = pRenderer.CreateTexture(pScreenSize.x, pScreenSize.y);
    pRenderer.UpdateTexture(pBufferId, pScreenSize.x, pScreenSize.y, pBuffer);

    pClock1 = std::chrono::system_clock::now();
    pClock2 = std::chrono::system_clock::now();

    if (pOnLaunch) {
      if (pOnLaunch(*this) != rcode::ok) pThreadRunning = false;
    }

    while (pThreadRunning) {
      while (pThreadRunning) {
        pClock2       = std::chrono::system_clock::now();
        pElapsedTimer = pClock2 - pClock1;
        pClock1       = pClock2;
        pElapsedTime  = pElapsedTimer.count();

        pFrameTimer += pElapsedTime;
        pFrameCount++;

        pPlatform.HandleSystemEvent();

        if (pFrameTimer >= 1.0f) {
          pFrameRate = pFrameCount;
          pFrameTimer -= 1.0f;

          pWindowTittle = pWindowName + " - FPS: " + std::to_string(pFrameRate);
          pPlatform.SetWindowTitle(pWindowTittle);

          pFrameCount = 0;
        }

        for (uint32_t i = 0; i < 3; i++) {
          pMouseButtons[i].pressed  = false;
          pMouseButtons[i].released = false;

          if (pMouseButtonsNew[i] != pMouseButtonsOld[i]) {
            if (pMouseButtonsNew[i]) {
              pMouseButtons[i].pressed = !pMouseButtons[i].held;
              pMouseButtons[i].held    = true;

            } else {
              pMouseButtons[i].released = true;
              pMouseButtons[i].held     = false;
            }
          }

          pMouseButtonsOld[i] = pMouseButtonsNew[i];
        }

        for (uint32_t i = 0; i < 256; i++) {
          pKeyboardKeys[i].pressed  = false;
          pKeyboardKeys[i].released = false;

          if (pKeyboardKeysNew[i] != pKeyboardKeysOld[i]) {
            if (pKeyboardKeysNew[i]) {
              pKeyboardKeys[i].pressed = !pKeyboardKeys[i].held;
              pKeyboardKeys[i].held    = true;

            } else {
              pKeyboardKeys[i].released = true;
              pKeyboardKeys[i].held     = false;
            }
          }

          pKeyboardKeysOld[i] = pKeyboardKeysNew[i];
        }

        if (pClearBuffer) {
          for (uint64_t i = 0; i < pScreenSize.prod(); i++) {
            pBuffer[i].n = pBufferColor.n;
          }
        }

        if (pOnUpdate) {
          if (pOnUpdate(*this) != rcode::ok) pThreadRunning = false;
        }

        pRenderer.UpdateViewport(pViewPos, pViewSize);
        pRenderer.ClearBuffer(Black, true);
        pRenderer.PrepareDrawing();

        pRenderer.ApplyTexture(pBufferId);
        pRenderer.UpdateTexture(pBufferId, pScreenSize.x, pScreenSize.y, pBuffer);
        pRenderer.DrawLayerQuad();

        for (auto& s : pSpritesPending) {
          pRenderer.ApplyTexture(s.pSprite->pBufferId);
          pRenderer.DrawDecalQuad(s);
        }

        pSpritesPending.clear();
        pRenderer.DisplayFrame();

        if (pWantsToClose) {
          pThreadRunning = false;
          pWantsToClose  = false;
        }
      }

      if (pOnClose) {
        if (pOnClose(*this) != rcode::ok) pThreadRunning = true;
      }
    }

    delete[] pBuffer;
    pRenderer.DeleteTexture(pBufferId);

    pPlatform.ThreadCleanUp();
    pHasBeenClosed = true;
  }

  rcode Application::Launch(bool background) {
    if (pHasBeenClosed) return rcode::abort;

    std::thread thread = std::thread(&pixel::Application::pStartThread, this);

    if (background) {
      thread.detach();

    } else {
      thread.join();
    }

    return rcode::ok;
  }

  void Application::pCreateFont() {
    std::string font_base64;

    // Base64 encoded 1805x32 monochromatic ascii font sheet. Using "Source Code Pro 32" as font

    font_base64 += "AAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAAAAAABCAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4D9wAID/AA";
    font_base64 += "AAAAAAOAAAAMABAAAAAIABAAAA/AAAAAcAABwA4AAOAPgPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAwAA6AAQAAAAAAAOAA8HAAAACAAQAAAAAAgAMAwAEOAAAAAAAAAAAAAAAAAABwAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4A8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPwHDADwHwAAAAAAAA4AAAA4AAAAAAAwAAAA4D8AAO";
    font_base64 += "AAAMADAB7AAQD/AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADwB8AB8AcAAAAAAAAcAB4Og";
    font_base64 += "GEAMAAeAIAPAHAAAByAAwAAAAAAAAAAAAAAAAAABsAHADgA+AHADwDAAfw/AH7A/x/ADwA/AAAAAAAAAAAA";
    font_base64 += "AAAAAP8DAAAAHAD/AwA/wH8A/H/g/wPwAwzg4P8D/h84wIEDAA94cIAD+AD8DwA+AP8DgB/w/x8GcBgAdwB";
    font_base64 += "wDuA4AA7/P4ABgAEAAAMADgAAAIADAAAABwAAAAAABgAAABwEAAAcAAB4AMADOAAAOAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAwAAAAAAAAAAAAAAAAAAAAABgA4AMABAAAAAACAA8DDATAMgB/gDxj4AwAOAMAB4AAAAAAAAAAAA";
    font_base64 += "AAAAAAA4AD+A/gHwP8A/gcAPID/B/A/+P8D/gPwDwAAAAAAAIAAAAACAODwAPADgAfg/wH4H/g/gP8P/H+A";
    font_base64 += "/4EBHPx/wP8DBzxwAOABDw5wwH+A/wfwH+D/AfwP/v/DAA4HYBwAjgMODuDg/wcwAHAAAGAAwAMAAADgAAA";
    font_base64 += "A4AAAAAAAwAAAAMABAACAAwAADgBwAAcAAAcAAAAAAAAAAAAAAAAAAAAAAIABAAAAAAAAAAAAAAAAAAAA4A";
    font_base64 += "AABwAwAAAAAAAAcAB4OACGAPwPhoGDYwDAAQAYADgAAAAAAAAAAAAAAAAAAAzg8QD/ADwc4OEBgAcwAAAfB";
    font_base64 += "wAw4OAAhwcAAAAAAAAYAADAAQAAHID/ANgAHHyAjweDDzAAgAMA+HwwgANwAABw4IADDgA84MEDDnwecOAB";
    font_base64 += "nwcceMDnA+AAGMDhAI4DwGHAwQEcAGAABgAMAAAMAGwAAAAACAAAABwAAAAAABgAAAAYAAAAcAAAAAAAAOA";
    font_base64 += "AAOAAAAAAAAAAAAAAAAAAAAAAAAAwAAAAAAAAAAAAAAAAAAAAAAwA4AAABgAAAAAAAA4ADwdAEMDDw2A4MA";
    font_base64 += "wAOACAAQAGACAAAAMAAAAAAAAAAIABHBgAHIABBwxwANgABgBwAAAABww4cOAAAAAAAADAAwAAeAAAAAM4O";
    font_base64 += "AAbgAMOeEBgwAMGAHAAgAMEBnAADgAADhw4wAGADT54wMEDBw5w8MCBAxw8YAAcAAM4HMBxABgcHHCAAQAO";
    font_base64 += "wACAAwCAAYANAAAAAAAAAIADAAAAAAADAAAAAwAAAA4AAAAAAAAcAAAcAAAAAAAAAAAAAAAAAAAAAAAABgA";
    font_base64 += "AAAAAAAAAAAAAAAAAAIABABwAwAAAAAAAAMAB4OAACAM4IBgMA4YDAAcAOADAAQAEAGAAAAAAAAAAAAA4wA";
    font_base64 += "EHgAMAwAAADoAbwAAADgAAcMABBgc4AA4AAAAAHAAAADwAAGCAAwxgB3DAgQcADHDAAAAOADgAwAAOwAEAw";
    font_base64 += "IGDAzgAsMEGHzg44MABDg44cICDAwCAA2AABwcYDoADhwEMOADgABgAYAAAMAA4AwAAAAAAAABwAAAAAABg";
    font_base64 += "AAAAYAAAAMABAAAAAACAAwCAAwAAAAAAAAAAAAAAAAAAAAAAwAAAAAAAAAAAAAAAAAAAAAAwAIADABgAAAA";
    font_base64 += "AAAA4ABgcgGEAAwCDMcAwAOAAAAMAMACAAAAMAAAAAAAAAAAAAxjAAHAAADgAwAE4AxgA4AAAAAY4wOAABu";
    font_base64 += "ADAB4A4AEAAAAPAAAOOIAB7gAOMHAAgAEcGADAAQAHABjAATgAADhwOAAHAD7YYAOHAzg4gOEADg5wcAAAc";
    font_base64 += "AAM4OCAw+Fw4DiAAwMADAADAAwAAAYAYwAAAAAAAPwAzgcAfgB8DMAP4P8H8H84PsD/AP4HcIADcADmcGD4";
    font_base64 += "APABGB8APgYYfoA/wP8fDmAwAOYA4DjAYAAM/n8ADgBwAAADAAAAAAAABwCDA/5/YABgMAMYBwAcAGAAAA4";
    font_base64 += "AMACAAQAAAAAAAAAAYAADOAAOAAAHADiAYwADABwAAGAABhgcwAB8AOADAA6A/w+ABwDAAAMwwBjAAQYGAD";
    font_base64 += "CAAwMAOABwAAADOAAHAAAHjgPgAMCPH+zgcAAHBzAcwMEBDg4AAA6AARwccDgcDjgDYHAAwAFgAIADAMAAY";
    font_base64 += "BwAAAAAAOB/wP0D8D/AvwH+B/z/gP8P8w/4H8D/AA44AA7Avh/MP4D/APMP4N8A4w/4H/j/wwEMDuAYHBwG";
    font_base64 += "HByAwf8PwAEADgBgAAAAAAAAwABgMMD/DxwADCMAdwCAAwAOAMABCIYAMAAAAAAAAAAAAA5wAAfAAQBgAIA";
    font_base64 += "DMAxgAIABAAAOwIGDAziADwB8APAA8P8B4AEAHHAABBgHOODgAAAGYGAAAAcADgBgAAfgAADgwHkAHAC4sY";
    font_base64 += "MZHA7A4ACGAzA4wIEDAMABMIADAwaHxwF3ABwGABwADABgAAAYAAYDAAAAAAAfDvjxAB8PPD7g4QEwADgcY";
    font_base64 += "McDgAMAHMCBA8ABeL6HHQ94PODHAx4fYJ+BgwdgADiAgQEcg4PBwQEDOADgADgAwAEADAAAAAAAABgADAbA";
    font_base64 += "MAAPAGMAwAcAYADAAAAwAM8eAAYAAAAAAAAAAMAAjuMAOAAADgA8AIcB7AcwPgDAAHA4cAAH4AGADwAHAAA";
    font_base64 += "AAHAAwAEGgIDjAAcPHADAABwMAOAAwAEADOAAHAAAHDgHgAMAN3Ywh8MAOBzgMAAOBzjwAQA4AAZw4MDA8D";
    font_base64 += "jABwDjAIABgAEADAAAA8BgAAAAAABAgAMPHHCAwAEHHjAABgADBzxwAHAAgAM4OAA4AMfj8MCBAw48cOCAA";
    font_base64 += "2wAOEAADAAHMHCA4fAwcBjgAAMADgAHADgAgAHAAQEAAAADgMEAGAbAB+AHAHgAAAwAGAAABsD/Af8fAADA";
    font_base64 += "/wcAAAAcwHEcAAcAwAH4A3AwgP8D9x8AHAD8AwzwAAAA4ABwAAAAAAAcABzAgB8wGOB/gAMAGICD/wP8Pxg";
    font_base64 += "AgP8fgAMAgAP3AXAA4OwOxnAYAIcDHAbA4cAD/AEAB8AADhwcGBsD8ADgDAA4ADAAgAMAYAAcHAAAAAAAAG";
    font_base64 += "DgAAcHADjAwAEOwABwwIADHAAOAHAAhwMAB+AwHA5wcICDAxwcYIAHAAcAgAHgAAYOOBwaBowDGHAA4ADgA";
    font_base64 += "AAHADAA/DAAAABgAAAAAEMA8AMgQIAHGAAAAAMAwADAD+D/AwAA+P8AAACAATieA+AAABwAfwAHBjDw4AcH";
    font_base64 += "gAOAP4CHHwAAAAAADgAAAAAAA8ABGHwDBgf8P3AAAANw8H+A/wcDf/D/A3AAAHDgdwAOAJzNwTgOA+Bw4ME";
    font_base64 += "AOPw/AP4A4AAYwAGDAWdjAA4A2AGAAwAGAGAAAAyAAQMAAAAAAAAMHODgAIADGBiAARgADhhwgAPAAQAO4D";
    font_base64 += "gA4AAMhsMBDgdwcIDDAQxwAOABADAAHMCAAYNj44A7AAcGAA4ADADgAAAOwDgGAAAADAAAACAIAPABAD/4g";
    font_base64 += "QMAAGAAADgA8AAAAwAAAAAAAAAAMADHcQAcAMABADxgwAAAHDzAATAAHB/gvwMAAAAAwAMAAAAAeAAYAMNh";
    font_base64 += "4OCAAw4OAGAADgYAcADg4A8GcAAOAAAOfA7AAYCzORjGYQAc/h8YAIf/AwB+ABwAAzjgMODsDOADAB8AOAD";
    font_base64 += "AAAAcAIABMGAAAAAAAAD4gQMcDABwAIMDMAADwAEDDnAAOADAAZwDAByAwXA4wOEADg5wOIABBgD4AAAGgA";
    font_base64 += "MYcHBgbBzgA+DAAOAA+AAAHACADxh+AAAAgAEAAID/DwB4ADGOczAAAAAMAAAHABsAYAAAAAAAAAAAAAfgA";
    font_base64 += "A6AAwA4AAAODhgAAIMDOAAHwIEH8HEAAAAAAOABAAAAgAMAA2AcDBwYcIDDAQAMwMEAAA4AHIDBAA7AAQDA";
    font_base64 += "gYcDOABwHAfDOQyAw/8AA+BwOAAAD4ADYAAHHAecnQFsAMABAAcAGAAAAwAwAAAAAAAAAADwP3AAgwEADmD";
    font_base64 += "w/wdgADBwwAEOAAcAOID7AIADMBgOBzgcgMEBDAcwwAAA/gHAAHAAAwwOjI0BOAAYHAAOAB8AgAMA4AEABw";
    font_base64 += "AAAAAAAADw/wEADjCGMRwHAACAAQBgAHAGAAwAAAAAAAAAAGAAHMABcACAAwCA4f8fAOBgAAfgABjAAQAGA";
    font_base64 += "AAAAAB4APx/ADwAAACMgYH/Bw5gOACAARgYAMABgAMwGMABOAAAOPBwAAcAjuNgMIcDMDgA4AAMDgcAgANw";
    font_base64 += "AAzgAOOAOzvAHQA4AHAAAAMAYAAABgAAAAAAAACADwYOYDAAwAEM/v8ADAAOBzjAAeAAAAfwOwBwAAbD4QC";
    font_base64 += "HAzA4gOEABhgAAP4AGAAOYIDDgLMzgA8AhwHgAQAHAHAAAAcAAAAAAAAAAAAAGAYAgAFHMAdnAAAAMAAADA";
    font_base64 += "DGAYABAAAAAAAAAAAOAAMYAA4AOAAAMPz/AwAcDMAAHIADOADAAAAAAAAAPID/D8ABAACAMTD4/8ABDA4AM";
    font_base64 += "IADAwA4AHAABgM4AAcAAAcOHOAAwHEcDO5wAAcHABzAwcEBAHAADoABHGAMYGMHHAcABwAHAGAAABwAwAAA";
    font_base64 += "AAAAAAAAOMDAAQ4GADiAwQEAgAGAfwAHOAAcAOAAPg4ADsBgOBzgcAAHBzgcwAADAAA8AAPAAQxgHHBnBrA";
    font_base64 += "BwDAAHADAAAAOAGAAAAAAAAAAAAAAAMMAADBwDObADwAAAAYAgAFgMAAwAAAAAAAAAADAAGAAA8ABgAMAAA";
    font_base64 += "4ADACAgwMcgAFwAAcAHAAAAAAAAA8AAAAeAAAAMA4HBxg4gMEBAAZwYAAABwAcwGAAB+AAAODAgQMcADiAg";
    font_base64 += "4EdDuDgAIADODg4AAAOwAFwgAGcAWz8gOMA4ADgAAAMAAADABgAAAAAAAAAAAMYOMDBAQAHMDgAADAA2APg";
    font_base64 += "AAeAAwAcwIMBwAEYDIcDHA7g4ACHAxhgAAAADmAAOIABjAFu7AB3ADgHwAEAOADAAQAMAAAAAAAAHAAAAGA";
    font_base64 += "YwAAHh8Ec8AAAAMAAADAABAQABgA4AAAAAB4AGAAccAA4ADgAAMAAgAEAOHCAAzAADuAAgAHgAQAHAIAHAA";
    font_base64 += "DgAABwAIa/YAAHBzh4AMAABwwA4ACAAxgM4AAcAAIMOOCAAwAHcDDwgwMOHADgAAMHDgjAATgADjgAO4CND";
    font_base64 += "zg4ABwADgCAAQBgAAADAAAAAAAAAGAAAwc4OADgAAYOAAAGgAMAHOAAcACAAzhwADgAg+FwgIMBHBzgcAAD";
    font_base64 += "DAAAwAEMAAY4gDOAjR1wHABmABwAAAcAOACAAQAAAAAAwAcAAAAMA3jwYGAYBz4AAAA4AAAHAAAAAACADwA";
    font_base64 += "AAOADgAMABwYAB8ADgAMcADBgAAccOAAHgAEcADgAPgDwAQDgAAAADgAAD8DgEQ7g4AAHHjAY8IABABwA4A";
    font_base64 += "CDARyAA+DAAQc4cADgAA4GfPDAgQMAHHDggAMHOAAHwAMHYAPw4QEDBoAD4AAAMAAAHABgAAAAAAAAAAAMc";
    font_base64 += "OCAAw4QOODAAwDAAHAAgAMcAA4AcAAHHAAHYDAcDnBwwIEDDhxwgAEAATiAA8CBB2AHsPEBBgfADsABAOAA";
    font_base64 += "AAcAMAAAAAAAAPgAAACAYQD+DwYM4+EfAAAABgDgAAAAAAAA8AEAAAB8ADAA4PEA4AA8APDhAQAGPHgAhwf";
    font_base64 += "gAPDggcMDwAcAPgAAEAAAQAAA4AEYAMABGBx4gI8Hgw8wAIADAHx8MIADcAA8PuAABw4AHMDBgA88HnAAAI";
    font_base64 += "8HHHDg4wPgAPB5AHwAHjxwwAFwAAwAAAYAAAMADAAAAAAAAACAhw98PICHBx8f8OAAGAAeAHCAA8ABAA7gA";
    font_base64 += "AfgIQyGwwEOPB7w8YCPDzAA8IED8CB43AB8ADY84OAA8AAcAAAcAOAAAAYAAAAAAAAfAAAAEAQAf4AAP/iP";
    font_base64 += "AwAAwAAADAAAAAAAAH4AAACADwAGAPgP8P/D/x/4HwDAAP8HwH8AHAD8H/A/APgAwA8AAAAAAAAAADwABgA";
    font_base64 += "YAIf/B+B/4P8A/j9wAAD+BwZw8P8B/wMcwMH/hwM4GOAB/wEOAMB/gAMc+D8AHAD8BwAPwIMHBnAADsD/H8";
    font_base64 += "AAAOAAgAEAAAAAAAAA4L+B/QPgf8B/A/wfAAOA/wcOcAA4AMABHOAA+IfBcDjAAf8B/g/gvwEGAPg/APwH/";
    font_base64 += "hkAD8CDBw44AB7A/x+AAQAcAMAAAAAAAADAAQAAAIIAAAMAwAN8YAAAADgAwAEAAAAAAIAPAAAA4ADgAAB8";
    font_base64 += "AP5/+P8D/AAAGAA/AOADgAMA/gD4AQAOAPABAAAAAAAAAAAHwACAA+DwPwDwA/wHwP8HDgAAP8AADv4/gB+";
    font_base64 += "AAzj4/3AABwM8gA/AAQDgB3CAA/wBgAMAfgDgAHjw4AAOwAH4/wMYAAAYADAAAAAAAAAAAPgxMD4A8APgYw";
    font_base64 += "D+AGAA4P/DAQ4ABwA4gAM4AHwwGA4HOIAPwPkA8DHAAAD8AQB+gA8D4ABw8OAADsAD+P8DMACAAwAYAAAAA";
    font_base64 += "AAAAAAAAAAAAGAAAAAAAAAAAAAGABgAAAAAAADAAQAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAA4AAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAwAAAwAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAdwAAAAAAAABwAAAAAAAAAAAA";
    font_base64 += "AAADgAAAAGAAAAAAAAAAAAAAAAAAAAAAAwAAAAAAYAcAAAAwAAAAAAAAAAAAAAAAAMAAAAAAAAAACAAYADA";
    font_base64 += "AAAAAAAGAAAAAAAwAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAAAAAAAAAAAAAOEAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAYAAA4ADAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAGAADgAAAAAA4AAAAAAAAAAAAAAAAAAHAADAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AABwAAAMAAAA4AYAAAAAAAAAAAAAAAAACAAQAAAAAAAAAAcAA4AAAAAAAAAAMAAAAAABgAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAYAAAAAAAAAAAAACABwMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAIAHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAABgAGAAAAP7/AAAAAAAAAAAAAAAAAAAAAAAOwAE";
    font_base64 += "AAAAAABwAAAAAAAAAAAAAAADgAAAAGAAAAAAAAAAAAAAAAAAAAAAAYAAAAAA4AMABAA4AAAAAAAAAAAAAAA";
    font_base64 += "AAMAAAAAAAAAAAAByAAwAAAAAAADAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYAAAAAA";
    font_base64 += "AAAAAAA4H8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADgDwAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAID/AAAD/gMAAMD/HwAAAAAAAAAAAAAAAAAAAADAATgAAAAAAMABAAAAAAAAAAAAAAAAHAAAAAM";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAA4AAAAA/gA4AP4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHOAAAAAAAAAAHAA";
    font_base64 += "AAAABwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOAAAAAAAAAAAAAAAOABAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8AEAAAAAAAAAAAAAAAAAAAAAAAAAAADwHwDgwH8AAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAcMADAAAAADA8AAAAAAAAAAAAAAAAgAMAAGAAAAAAAAAAAAAAAAAAAAAAAOAAAAAAgB8";
    font_base64 += "AB8APAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAIAAAAAAAB4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPwfAAAAAAD+AwAA";
    font_base64 += "AAAAAAAAAAAAAHAAAAAMAAAAAAAAAAAAAAAAAAAAAMAPAAAAAAAA4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD+AAAAAACAHwAAAAAAAAAAAAAAAAAOAACAAQAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAD4AAAAAAAAABwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    font_base64 += "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIADAAAAAAA=";

    uint8_t font_data[1805 * 32 / 8];

    int8_t b64invs[] = {62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1,
                        -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
                        18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
                        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

    uint32_t i;
    uint32_t j;
    uint32_t v;

    for (i = 0, j = 0; i < font_base64.length(); i += 4, j += 3) {
      v = b64invs[font_base64[i] - 43];
      v = (v << 6) | b64invs[font_base64[i + 1] - 43];

      v = font_base64[i + 2] == '=' ? v << 6 : (v << 6) | b64invs[font_base64[i + 2] - 43];
      v = font_base64[i + 3] == '=' ? v << 6 : (v << 6) | b64invs[font_base64[i + 3] - 43];

      font_data[j] = (v >> 16) & 0xFF;

      if (font_base64[i + 2] != '=') {
        font_data[j + 1] = (v >> 8) & 0xFF;
      }

      if (font_base64[i + 3] != '=') {
        font_data[j + 2] = v & 0xFF;
      }
    }

    pFontSprite = new Sprite(1805, 32);

    for (uint16_t m = 0; m < 1805 * 32; m++) {
      pFontSprite->pBuffer[m].n = (font_data[m / 8] & (1 << (m % 8)) ? 0xFFFFFFFF : 0x00000000);
    }
  }

  void Application::Close() { pThreadRunning = false; }

  void Application::EnsureClosed() {
    while (!pHasBeenClosed) {
    }
  }

  void Application::SetName(const std::string& name) { pWindowName = name; }

  void Application::SetDrawingMode(pixel::DrawingMode mode) { pDrawingMode = mode; }

  void Application::RegisterSprite(Sprite* spr) {
    if (spr->pBufferId != 0xFFFFFFFF) pRenderer.DeleteTexture(spr->pBufferId);

    spr->pBufferId = pRenderer.CreateTexture(spr->pSize.x, spr->pSize.y);
    pRenderer.UpdateTexture(spr->pBufferId, spr);
  }

  void Application::DrawString(const vu2d& pos, std::string_view text, uint8_t size, const Pixel& color) {
    vu2d p = pos;

    for (const char& c : text) {
      if (isascii(c) > 0) {
        if (c == '\n') {
          p.y += size;
          p.x = pos.x;

          continue;
        }

        DrawPartialSprite(p,
                          vu2d((c - 32) * 19, 0),
                          vu2d(19, 32),
                          pFontSprite,
                          vf2d((float)size / 32.0f, (float)size / 32.0f),
                          color);
        p.x += (float)size * 0.6f;
      }
    }
  }

  void Application::Draw(const vu2d& pos, const Pixel& pixel) {
    if (pos.x >= pScreenSize.x) return;
    if (pos.y >= pScreenSize.y) return;

    if (pDrawingMode == DrawingMode::FULL_ALPHA) {
      Pixel d = pBuffer[pos.y * pScreenSize.x + pos.x];

      float a = (float)(pixel.v.a / 255.0f);
      float c = 1.0f - a;

      uint8_t r = a * (float)pixel.v.r + c * (float)d.v.r;
      uint8_t g = a * (float)pixel.v.g + c * (float)d.v.g;
      uint8_t b = a * (float)pixel.v.b + c * (float)d.v.b;

      pBuffer[pos.y * pScreenSize.x + pos.x].v = {r, g, b, 255};
      // pBuffer[pos.y * pScreenSize.x + pos.x].n = (uint8_t)r | ((uint8_t)g << 8) | ((uint8_t)b << 16) | (255 << 24);

    } else if (pDrawingMode == DrawingMode::NO_ALPHA) {
      pBuffer[pos.y * pScreenSize.x + pos.x] = pixel;

    } else if (pixel.v.a == 255) {
      pBuffer[pos.y * pScreenSize.x + pos.x] = pixel;
    }
  }

  void Application::DrawLine(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
    int32_t x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = pos2.x - pos1.x;
    dy = pos2.y - pos1.y;

    if (dx == 0) {
      if (pos2.y < pos1.y)
        for (y = pos2.y; y <= (int32_t)pos1.y; y++) Draw(vu2d(pos2.x, y), pixel);
      else
        for (y = pos1.y; y <= (int32_t)pos2.y; y++) Draw(vu2d(pos1.x, y), pixel);

      return;
    }

    if (dy == 0) {
      if (pos2.x < pos1.x)
        for (x = pos2.x; x <= (int32_t)pos1.x; x++) Draw(vu2d(x, pos2.y), pixel);
      for (x = pos1.x; x <= (int32_t)pos2.x; x++) Draw(vu2d(x, pos1.y), pixel);

      return;
    }

    dx1 = std::abs(dx);
    dy1 = std::abs(dy);
    px  = 2 * dy1 - dx1;
    py  = 2 * dx1 - dy1;

    if (dy1 <= dx1) {
      if (dx >= 0) {
        x  = pos1.x;
        y  = pos1.y;
        xe = pos2.x;
      } else {
        x  = pos2.x;
        y  = pos2.y;
        xe = pos1.x;
      }

      Draw(vu2d(x, y), pixel);

      for (i = 0; x < xe; i++) {
        x = x + 1;

        if (px < 0)
          px = px + 2 * dy1;
        else {
          if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
            y = y + 1;
          else
            y = y - 1;
          px = px + 2 * (dy1 - dx1);
        }

        Draw(vu2d(x, y), pixel);
      }

    } else {
      if (dy >= 0) {
        x  = pos1.x;
        y  = pos1.y;
        ye = pos2.y;
      } else {
        x  = pos2.x;
        y  = pos2.y;
        ye = pos1.y;
      }

      Draw(vu2d(x, y), pixel);

      for (i = 0; y < ye; i++) {
        y = y + 1;

        if (py <= 0) {
          py = py + 2 * dx1;
        } else {
          if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
            x = x + 1;
          else
            x = x - 1;
          py = py + 2 * (dx1 - dy1);
        }

        Draw(vu2d(x, y), pixel);
      }
    }
  }

  void Application::DrawCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel) {
    uint32_t x0 = 0;
    uint32_t y0 = radius;
    int      d  = 3 - 2 * radius;

    if (!radius) return;

    while (y0 >= x0) {
      Draw(vu2d(pos.x + x0, pos.y - y0), pixel);
      Draw(vu2d(pos.x + y0, pos.y - x0), pixel);
      Draw(vu2d(pos.x + y0, pos.y + x0), pixel);
      Draw(vu2d(pos.x + x0, pos.y + y0), pixel);
      Draw(vu2d(pos.x - x0, pos.y + y0), pixel);
      Draw(vu2d(pos.x - y0, pos.y + x0), pixel);
      Draw(vu2d(pos.x - y0, pos.y - x0), pixel);
      Draw(vu2d(pos.x - x0, pos.y - y0), pixel);

      if (d < 0)
        d += 4 * x0++ + 6;
      else
        d += 4 * (x0++ - y0--) + 10;
    }
  }

  void Application::FillCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel) {
    int x0 = 0;
    int y0 = radius;
    int d  = 3 - 2 * radius;

    if (!radius) return;

    auto scanline = [&](int sx, int ex, int ny) {
      for (int i = sx; i <= ex; i++) Draw(vu2d(i, ny), pixel);
    };

    while (y0 >= x0) {
      scanline(pos.x - x0, pos.x + x0, pos.y - y0);
      scanline(pos.x - y0, pos.x + y0, pos.y - x0);
      scanline(pos.x - x0, pos.x + x0, pos.y + y0);
      scanline(pos.x - y0, pos.x + y0, pos.y + x0);

      if (d < 0)
        d += 4 * x0++ + 6;
      else
        d += 4 * (x0++ - y0--) + 10;
    }
  }

  void Application::DrawRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
    DrawLine(vu2d(pos1.x, pos1.y), vu2d(pos1.y, pos2.x), pixel);
    DrawLine(vu2d(pos1.y, pos2.x), vu2d(pos2.x, pos2.y), pixel);
    DrawLine(vu2d(pos2.x, pos2.y), vu2d(pos2.y, pos1.x), pixel);
    DrawLine(vu2d(pos2.y, pos1.x), vu2d(pos1.x, pos1.y), pixel);
  }

  void Application::FillRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
    for (uint32_t x = std::min(pos1.x, pos2.x); x <= std::max(pos1.x, pos2.x); x++) {
      for (uint32_t y = std::min(pos1.y, pos2.y); y <= std::max(pos1.y, pos2.y); y++) {
        Draw(vu2d(x, y), pixel);
      }
    }
  }

  void Application::DrawTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel) {
    DrawLine(vu2d(pos1.x, pos1.y), vu2d(pos2.x, pos2.y), pixel);
    DrawLine(vu2d(pos2.x, pos2.y), vu2d(pos3.x, pos3.y), pixel);
    DrawLine(vu2d(pos3.x, pos3.y), vu2d(pos1.x, pos1.y), pixel);
  }

  void Application::FillTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel) {
    auto drawline = [&](int32_t sx, int32_t ex, int32_t ny) {
      for (int32_t i = sx; i <= ex; i++) Draw(vu2d(i, ny), pixel);
    };

    vu2d p1 = pos1;
    vu2d p2 = pos2;
    vu2d p3 = pos3;

    int32_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
    int32_t signx1, signx2, dx1, dy1, dx2, dy2;
    int32_t e1, e2;

    bool changed1 = false;
    bool changed2 = false;

    if (p1.y > p2.y) {
      std::swap(p1.y, p2.y);
      std::swap(p1.x, p2.x);
    }
    if (p1.y > p3.y) {
      std::swap(p1.y, p3.y);
      std::swap(p1.x, p3.x);
    }
    if (p2.y > p3.y) {
      std::swap(p2.y, p3.y);
      std::swap(p2.x, p3.x);
    }

    t1x = t2x = p1.x;
    y         = p1.y;
    dx1       = (int)(p2.x - p1.x);
    if (dx1 < 0) {
      dx1    = -dx1;
      signx1 = -1;
    } else
      signx1 = 1;
    dy1 = (int)(p2.y - p1.y);

    dx2 = (int)(p3.x - p1.x);
    if (dx2 < 0) {
      dx2    = -dx2;
      signx2 = -1;
    } else
      signx2 = 1;
    dy2 = (int)(p3.y - p1.y);

    if (dy1 > dx1) {
      std::swap(dx1, dy1);
      changed1 = true;
    }
    if (dy2 > dx2) {
      std::swap(dy2, dx2);
      changed2 = true;
    }

    e2 = (int)(dx2 >> 1);

    if (p1.y == p2.y) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
      t1xp = 0;
      t2xp = 0;
      if (t1x < t2x) {
        minx = t1x;
        maxx = t2x;
      } else {
        minx = t2x;
        maxx = t1x;
      }

      while (i < dx1) {
        i++;
        e1 += dy1;

        while (e1 >= dx1) {
          e1 -= dx1;

          if (changed1)
            t1xp = signx1;
          else
            goto next1;
        }

        if (changed1)
          break;
        else
          t1x += signx1;
      }

    next1:

      while (1) {
        e2 += dy2;

        while (e2 >= dx2) {
          e2 -= dx2;

          if (changed2)
            t2xp = signx2;
          else
            goto next2;
        }

        if (changed2)
          break;
        else
          t2x += signx2;
      }

    next2:

      if (minx > t1x) minx = t1x;
      if (minx > t2x) minx = t2x;
      if (maxx < t1x) maxx = t1x;
      if (maxx < t2x) maxx = t2x;

      drawline(minx, maxx, y);

      if (!changed1) t1x += signx1;
      t1x += t1xp;

      if (!changed2) t2x += signx2;
      t2x += t2xp;

      y += 1;
      if (y == (int32_t)p2.y) break;
    }

  next:

    dx1 = (int)(p3.x - p2.x);
    if (dx1 < 0) {
      dx1    = -dx1;
      signx1 = -1;
    } else
      signx1 = 1;
    dy1 = (int)(p3.y - p2.y);

    t1x = p2.x;

    if (dy1 > dx1) {
      std::swap(dy1, dx1);
      changed1 = true;
    } else
      changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
      t1xp = 0;
      t2xp = 0;

      if (t1x < t2x) {
        minx = t1x;
        maxx = t2x;
      } else {
        minx = t2x;
        maxx = t1x;
      }

      while (i < dx1) {
        e1 += dy1;

        while (e1 >= dx1) {
          e1 -= dx1;
          if (changed1) {
            t1xp = signx1;
            break;
          } else
            goto next3;
        }

        if (changed1)
          break;
        else
          t1x += signx1;
        if (i < dx1) i++;
      }

    next3:

      while (t2x != (int32_t)p3.x) {
        e2 += dy2;

        while (e2 >= dx2) {
          e2 -= dx2;
          if (changed2)
            t2xp = signx2;
          else
            goto next4;
        }

        if (changed2)
          break;
        else
          t2x += signx2;
      }

    next4:

      if (minx > t1x) minx = t1x;
      if (minx > t2x) minx = t2x;
      if (maxx < t1x) maxx = t1x;
      if (maxx < t2x) maxx = t2x;

      drawline(minx, maxx, y);

      if (!changed1) t1x += signx1;
      t1x += t1xp;

      if (!changed2) t2x += signx2;
      t2x += t2xp;

      y += 1;
      if (y > (int32_t)p3.y) return;
    }
  }

  void Application::DrawSprite(const vu2d& pos, Sprite* spr, const vf2d& scale, const Pixel& tint) {
    SpriteRef spr_ref;
    spr_ref.pSprite = spr;

    vf2d newpos = {(float(pos.x) * pInvScreenSize.x) * 2.0f - 1.0f,
                   ((float(pos.y) * pInvScreenSize.y) * 2.0f - 1.0f) * -1.0f};

    vf2d newsize = {newpos.x + (2.0f * float(spr->pSize.x) * pInvScreenSize.x) * scale.x,
                    newpos.y - (2.0f * float(spr->pSize.y) * pInvScreenSize.y) * scale.y};

    spr_ref.pTint = tint;

    spr_ref.pPos[0] = {newpos.x, newpos.y};
    spr_ref.pPos[1] = {newpos.x, newsize.y};
    spr_ref.pPos[2] = {newsize.x, newsize.y};
    spr_ref.pPos[3] = {newsize.x, newpos.y};

    pSpritesPending.push_back(spr_ref);
  }

  void Application::DrawPartialSprite(const vu2d&  pos,
                                      const vu2d&  spos,
                                      const vu2d&  ssize,
                                      Sprite*      spr,
                                      const vf2d&  scale,
                                      const Pixel& tint) {
    SpriteRef spr_ref;
    spr_ref.pSprite = spr;

    vf2d newpos = {(float(pos.x) * pInvScreenSize.x) * 2.0f - 1.0f,
                   ((float(pos.y) * pInvScreenSize.y) * 2.0f - 1.0f) * -1.0f};

    vf2d newsize = {newpos.x + (2.0f * (float)ssize.x * pInvScreenSize.x) * scale.x,
                    newpos.y - (2.0f * (float)ssize.y * pInvScreenSize.y) * scale.y};

    spr_ref.pTint = tint;

    spr_ref.pPos[0] = {newpos.x, newpos.y};
    spr_ref.pPos[1] = {newpos.x, newsize.y};
    spr_ref.pPos[2] = {newsize.x, newsize.y};
    spr_ref.pPos[3] = {newsize.x, newpos.y};

    vf2d uvtl = (vf2d)spos / (vf2d)spr->pSize * spr->pUvScale;
    vf2d uvbr = uvtl + ((vf2d)ssize / (vf2d)spr->pSize * spr->pUvScale);

    spr_ref.pUv[0] = {uvtl.x, uvtl.y};
    spr_ref.pUv[1] = {uvtl.x, uvbr.y};
    spr_ref.pUv[2] = {uvbr.x, uvbr.y};
    spr_ref.pUv[3] = {uvbr.x, uvtl.y};

    pSpritesPending.push_back(spr_ref);
  }

  // void Application::DrawWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& pos, const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  // void Application::DrawPartialWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& post, const vf2d& spos, const vf2d&
  // ssize, const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  // void Application::DrawRotatedSprite(const vf2d& pos, uint8_t sprite, float alpha, const vf2d& center = vf2d(0.0f,
  // 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  // void Application::DrawPartialRotatedSprite(const vf2d& pos, uint8_t sprite, float alpha, const vf2d& spos, const
  // vf2d& ssize, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White)
  // {
  //   //! STUB: Implement function.
  // }

  void Application::UpdateMouse(uint32_t x, uint32_t y) {
    pHasMouseFocus = true;

    x -= pViewPos.x;
    y -= pViewPos.y;

    pMousePos.x = (((float)x / (float)(pWindowSize.x - (pViewPos.x * 2)) * (float)pScreenSize.x));
    pMousePos.y = (((float)y / (float)(pWindowSize.y - (pViewPos.y * 2)) * (float)pScreenSize.y));
  }

  void Application::UpdateMouseWheel(uint32_t delta) { pMouseWheel += delta; }

  void Application::UpdateWindowSize(uint32_t x, uint32_t y) {
    pWindowSize = vu2d(x, y);
    UpdateViewport();
  }

  void Application::UpdateViewport() {
    uint32_t ww   = pScreenSize.x * pScale;
    uint32_t wh   = pScreenSize.y * pScale;
    float    wasp = (float)ww / (float)wh;

    pViewSize.x = (uint32_t)pWindowSize.x;
    pViewSize.y = (uint32_t)((float)pViewSize.x / wasp);

    if (pViewSize.y > pWindowSize.y) {
      pViewSize.y = pWindowSize.y;
      pViewSize.x = (uint32_t)((float)pViewSize.y * wasp);
    }

    pViewPos = (pWindowSize - pViewSize) / 2;
  }

  void Application::UpdateMouseState(uint32_t button, bool state) { pMouseButtonsNew[button] = state; }
  void Application::UpdateKeyState(uint32_t key, bool state) { pKeyboardKeysNew[key] = state; }
  void Application::UpdateMouseFocus(bool state) { pHasMouseFocus = state; }
  void Application::UpdateKeyFocus(bool state) { pHasInputFocus = state; }

  pixel::DrawingMode Application::DrawingMode() const { return pDrawingMode; }

  const vu2d& Application::ScreenSize() const { return pScreenSize; }
  const vu2d& Application::WindowSize() const { return pWindowSize; }
  const vu2d& Application::WindowPos() const { return pWindowPos; }
  const vu2d& Application::MousePos() const { return pMousePos; }

  uint32_t      Application::MouseWheel() const { return pMouseWheel; }
  const Button& Application::Mouse(pixel::Mouse button) const { return pMouseButtons[(uint8_t)button]; }

  const Button& Application::Key(pixel::Key key) const { return pKeyboardKeys[(uint8_t)key]; }

  float    Application::et() const { return pElapsedTime; }
  uint32_t Application::fps() const { return pFrameRate; }

  namespace {
    template <typename _Call>
    union storage {
      storage() {}
      std::decay_t<_Call> callable;
    };

    template <int, typename _Call, typename _Ret, typename... _Args>
    auto fun(_Call&& c, _Ret (*)(_Args...)) {
      static bool           used = false;
      static storage<_Call> s;
      using type = decltype(s.callable);

      if (used) {
        s.callable.~type();
      }

      new (&s.callable) type(std::forward<_Call>(c));
      used = true;

      return [](_Args... args) -> _Ret { return _Ret(s.callable(std::forward<_Args>(args)...)); };
    }
  }

  // A very hackish way of allowing the user to pass an inline declared capturing lambda as a function pointer in the
  // parameter list
  template <typename Fn = rcode(Application&), int N = 0, typename _Call>
  Fn* fn(_Call&& c) {
    return fun<N>(std::forward<_Call>(c), (Fn*)nullptr);
  }
}

/*
____________________________

  Renderer implementations
____________________________

*/

namespace pixel {
  rcode Renderer::CreateDevice(GLFWwindow* window, bool fullscreen, bool vsync) {
    IGNORE(fullscreen);

    pWindow = window;
    glfwMakeContextCurrent(pWindow);

    if (!vsync) glfwSwapInterval(0);

    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return pixel::ok;
  }

  void Renderer::DisplayFrame() { glfwSwapBuffers(pWindow); }

  void Renderer::PrepareDrawing() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void Renderer::DrawLayerQuad() {
    glBegin(GL_QUADS);
    glColor4ub(255, 255, 255, 255);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glEnd();
  }

  void Renderer::DrawDecalQuad(const SpriteRef& sprite) {
    glBegin(GL_QUADS);

    glColor4ub(sprite.pTint.v.r, sprite.pTint.v.g, sprite.pTint.v.b, sprite.pTint.v.a);

    glTexCoord4f(sprite.pUv[0].x, sprite.pUv[0].y, 0.0f, sprite.pW[0]);
    glVertex2f(sprite.pPos[0].x, sprite.pPos[0].y);
    glTexCoord4f(sprite.pUv[1].x, sprite.pUv[1].y, 0.0f, sprite.pW[1]);
    glVertex2f(sprite.pPos[1].x, sprite.pPos[1].y);
    glTexCoord4f(sprite.pUv[2].x, sprite.pUv[2].y, 0.0f, sprite.pW[2]);
    glVertex2f(sprite.pPos[2].x, sprite.pPos[2].y);
    glTexCoord4f(sprite.pUv[3].x, sprite.pUv[3].y, 0.0f, sprite.pW[3]);
    glVertex2f(sprite.pPos[3].x, sprite.pPos[3].y);

    glEnd();
  }

  uint32_t Renderer::CreateTexture(uint32_t width, uint32_t height) {
    IGNORE(width);
    IGNORE(height);

    uint32_t id = 0;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    return id;
  }

  uint32_t Renderer::DeleteTexture(uint32_t id) {
    glDeleteTextures(1, &id);
    return id;
  }

  void Renderer::UpdateTexture(uint32_t id, Sprite* spr) {
    IGNORE(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr->pSize.x, spr->pSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, spr->pBuffer);
  }

  void Renderer::UpdateTexture(uint32_t id, uint32_t w, uint32_t h, Pixel* buffer) {
    IGNORE(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
  }

  void Renderer::ApplyTexture(uint32_t id) { glBindTexture(GL_TEXTURE_2D, id); }

  void Renderer::UpdateViewport(const vu2d& pos, const vu2d& size) { glViewport(pos.x, pos.y, size.x, size.y); }

  void Renderer::ClearBuffer(Pixel p, bool depth) {
    glClearColor(float(p.v.r) / 255.0f, float(p.v.g) / 255.0f, float(p.v.b) / 255.0f, float(p.v.a) / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
  }

}

/*
____________________________

  Platform implementations
____________________________

*/

#include <png.h>

namespace pixel {
  void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    ((std::istream*)a)->read((char*)data, length);
  }

  rcode FileUtil::LoadImage(Sprite* spr, const std::string& filename) {
    // if (std::filesystem::exists(filename)) return rcode::file_err;
    if (spr->pBuffer != nullptr) delete[] spr->pBuffer;

    png_structp png;
    png_infop   info;

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png) {
      spr->pSize   = {0, 0};
      spr->pBuffer = nullptr;

      return rcode::err;
    }

    info = png_create_info_struct(png);

    if (!info) {
      spr->pSize   = {0, 0};
      spr->pBuffer = nullptr;

      return rcode::err;
    }

    if (setjmp(png_jmpbuf(png))) {
      spr->pSize   = {0, 0};
      spr->pBuffer = nullptr;

      return rcode::err;
    }

    FILE* f = fopen(filename.c_str(), "r");
    if (!f) return rcode::file_err;

    png_init_io(png, f);
    png_read_info(png, info);

    png_byte   color_type;
    png_byte   bit_depth;
    png_bytep* row_pointers;

    spr->pSize = vu2d(png_get_image_width(png, info), png_get_image_height(png, info));
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) {
      png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
      png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * spr->GetSize().y);

    for (uint32_t y = 0; y < spr->GetSize().y; y++) {
      row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);
    spr->pBuffer = new Pixel[spr->GetSize().x * spr->GetSize().y];

    for (uint32_t y = 0; y < spr->GetSize().y; y++) {
      png_bytep row = row_pointers[y];

      for (uint32_t x = 0; x < spr->GetSize().x; x++) {
        png_bytep px                           = &(row[x * 4]);
        spr->pBuffer[y * spr->GetSize().x + x] = Pixel(px[0], px[1], px[2], px[3]);
      }
    }

    for (uint32_t y = 0; y < spr->GetSize().y; y++) {
      free(row_pointers[y]);
    }

    free(row_pointers);
    png_destroy_read_struct(&png, &info, nullptr);

    fclose(f);

    return rcode::ok;
  }

  rcode FileUtil::SaveImage(Sprite* spr, const std::string& filename) {
    IGNORE(spr);
    IGNORE(filename);

    return rcode::ok;
  }

  void Platform::ApplicationStartUp() { glfwInit(); }

  void Platform::ApplicationCleanUp() {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
  }

  rcode Platform::CreateGraphics(bool fullscreen, bool vsync, const vu2d& viewpos, const vu2d& viewsize) {
    if (App->pRenderer.CreateDevice(pWindow, fullscreen, vsync) == rcode::ok) {
      App->pRenderer.UpdateViewport(viewpos, viewsize);
      return rcode::ok;
    } else {
      return rcode::err;
    }
  }

  rcode Platform::CreateWindowPane(const vu2d& winpos, vu2d& winsize, bool fullscreen) {
    pWindow = glfwCreateWindow(winsize.x, winsize.y, "Pixel Engine", NULL, NULL);
    glfwSetWindowPos(pWindow, winpos.x, winpos.y);

    return rcode::ok;
  }

  void Platform::SetWindowTitle(const std::string& s) { glfwSetWindowTitle(pWindow, s.c_str()); }

  void Platform::StartSystemEventLoop() {
    while (!App->pHasBeenClosed) {
      glfwPollEvents();
    }
  }

  void Platform::HandleSystemEvent() {
    // TODO: Handle window resizing, mouse and keyboard
    App->pWantsToClose = glfwWindowShouldClose(pWindow);
  }
}
