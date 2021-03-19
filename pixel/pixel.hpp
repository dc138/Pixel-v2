/*
              _________________________

                The Pixel Library
              _________________________


  ABOUT:
  ~~~~~

  A single file object oriented library to more easily create fast graphical
  c++ applications that abstracts the  creation of windows and makes usng
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
# define PIXEL_LINUX
#elif defined(_WIN32)
# define PIXEL_WIN
#elif defined(__APPLE__)
# define PIXEL_MACOS
#else
# error "Unsupported platform."
#endif

#ifndef PIXEL_USE_DIRECTX
# define PIXEL_USE_OPENGL
#else
# ifdef PIXEL_LINUX
#   error "DirectX cannot be used with a linux build."
# endif
#endif

#define PIXEL_VERSION_MAJOR 2
#define PIXEL_VERSION_MINOR 2
#define PIXEL_VERSION_PATCH 0

#define IGNORE(x) (void(x))

#include <string>
#include <chrono>
#include <map>
#include <istream>
#include <atomic>
#include <vector>
#include <thread>
#include <filesystem>
#include <cmath>

/*
___________________________

  Header declarations
___________________________

*/

#ifdef PIXEL_USE_DIRECTX
# error "Currently unsupported renderer."
#endif /* PIXEL_USE_DIRECTX */

#ifdef PIXEL_USE_OPENGL
# ifdef PIXEL_LINUX
#   include <GL/gl.h>

    namespace X11 {
#     include <GL/glx.h>
#     include <X11/X.h>
#     include <X11/Xlib.h>
    }

    typedef int(glSwapInterval_t)(X11::Display* dpy, X11::GLXDrawable drawable, int interval);
    static glSwapInterval_t* glSwapIntervalEXT;

# elif defined(PIXEL_WIN)
# elif defined(PIXEL_MACOS)
# endif
#endif /* PIXEL_USE_OPENGL */

namespace pixel {
  enum rcode { ok = 0, err = 1, file_err = 2, abort = 3 };
  class Application;
  class Sprite;

  struct Button {
    bool pressed = false;
    bool held = false;
    bool released = false;
  };

  enum class Key: uint8_t {
    NONE,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    UP, DOWN, LEFT, RIGHT,
    SPACE, TAB, SHIFT, CTRL, INS, DEL, HOME, END, PGUP, PGDN,
    BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
    NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
    NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD
  };

  enum class Mouse: uint8_t {
    LEFT = 0,
    MIDDLE = 1,
    RIGHT = 2
  };

  enum class DrawingMode: uint8_t {
    NO_ALPHA, FULL_ALPHA, MASK
  };

  template<class T> struct v2d {
    T x = 0; T y = 0;

    v2d(): x(0), y(0) {}
    v2d(T x, T y): x(x), y(y) {}
    v2d(const v2d& v): x(v.x), y(v.y) {}

    T prod() const noexcept { return x * y; }
    T mod() const noexcept { return sqrt((x*x) + (y*y)); }

    v2d operator + (const T& rhs) const noexcept { return v2d(this->x + rhs, this->y + rhs); }
    v2d operator - (const T& rhs) const noexcept { return v2d(this->x - rhs, this->y - rhs); }
    v2d operator * (const T& rhs) const noexcept { return v2d(this->x * rhs, this->y * rhs); }
    v2d operator / (const T& rhs) const noexcept { return v2d(this->x / rhs, this->y / rhs); }
    v2d operator % (const T& rhs) const noexcept { return v2d(this->x % rhs, this->y % rhs); }

    v2d operator + (const v2d& rhs) const noexcept { return v2d(this->x + rhs.x, this->y + rhs.y); }
    v2d operator - (const v2d& rhs) const noexcept { return v2d(this->x - rhs.x, this->y - rhs.y); }
    v2d operator * (const v2d& rhs) const noexcept { return v2d(this->x * rhs.x, this->y * rhs.y); }
    v2d operator / (const v2d& rhs) const noexcept { return v2d(this->x / rhs.x, this->y / rhs.y); }
    v2d operator % (const v2d& rhs) const noexcept { return v2d(this->x % rhs.x, this->y % rhs.y); }

    v2d& operator += (const T& rhs) noexcept { this->x += rhs; this->y += rhs; return *this; }
    v2d& operator -= (const T& rhs) noexcept { this->x -= rhs; this->y -= rhs; return *this; }
    v2d& operator *= (const T& rhs) noexcept { this->x *= rhs; this->y *= rhs; return *this; }
    v2d& operator /= (const T& rhs) noexcept { this->x /= rhs; this->y /= rhs; return *this; }

    v2d& operator += (const v2d& rhs) noexcept { this->x += rhs.x; this->y += rhs.y; return *this; }
    v2d& operator -= (const v2d& rhs) noexcept { this->x -= rhs.x; this->y -= rhs.y; return *this; }
    v2d& operator *= (const v2d& rhs) noexcept { this->x *= rhs.x; this->y *= rhs.y; return *this; }
    v2d& operator /= (const v2d& rhs) noexcept { this->x /= rhs.x; this->y /= rhs.y; return *this; }

    template<typename U> operator v2d<U>() const noexcept { return { static_cast<U>(this->x), static_cast<U>(this->y) }; }
  };

  typedef v2d<int32_t > vi2d;
  typedef v2d<uint32_t> vu2d;
  typedef v2d<uint16_t> vs2d;
  typedef v2d<uint8_t > vc2d;
  typedef v2d<double  > vd2d;
  typedef v2d<float   > vf2d;

  struct Pixel {
    union {
      uint32_t n = 0x000000FF;
      struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; } v;
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

  Pixel RandPixel() {
    return Pixel(rand() % 255, rand() % 255, rand() % 255, rand() % 255);
  }

  class Sprite final {
  public:
    Sprite(const std::string& filename);
    Sprite(uint32_t w, uint32_t h);
    ~Sprite();

    friend class Application;
    friend class Helper;
    friend class Renderer;
    friend class Platform;

  public:
    Sprite(const Sprite& src);
    Sprite& operator=(const Sprite& rhs);

    Sprite(Sprite&& src) noexcept;
    Sprite& operator=(Sprite&& rhs) noexcept;

  public:
    Pixel GetPixel(uint32_t x, uint32_t y) const;
    void SetPixel(uint32_t x, uint32_t y, const Pixel& p);

    vu2d GetSize() const;
    void SetSize(uint32_t w, uint32_t h);

  public:
    rcode LoadFromFile(const std::string& filename);
    rcode SaveToFile(const std::string& filename);

  public:
    void Swap(Sprite& other) noexcept;

  private:
    vu2d pSize;
    vf2d pUvScale = vf2d(1.0f, 1.0f);

    Pixel* pBuffer = nullptr;
    uint32_t pBufferId = 0xFFFFFFFF;

    vf2d pPos[4] = { vf2d(0.0f, 0.0f), vf2d(0.0f, 0.0f), vf2d(0.0f, 0.0f), vf2d(0.0f, 0.0f) };
    vf2d pUv[4] = { vf2d(0.0f, 0.0f), vf2d(0.0f, 1.0f), vf2d(1.0f, 1.0f), vf2d(1.0f, 0.0f) };
    float pW[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    Pixel pTint = White;
  };

  class Helper final {
  public:
    static rcode LoadImage(Sprite* spr, const std::string& filename);
    static rcode SaveImage(Sprite* spr, const std::string& filename);
  };

  class Renderer final {
  private:

#ifdef PIXEL_USE_OPENGL
# ifdef PIXEL_LINUX

    X11::Display* pDisplay = nullptr;
    X11::Window* pWindow = nullptr;
    X11::XVisualInfo* pVisualInfo = nullptr;
    X11::GLXContext pContext = nullptr;

# elif defined(PIXEL_WIN)
# elif defined(PIXEL_MACOS)
# endif
#endif

  public:
    rcode CreateDevice(std::vector<void*> params, bool fullscreen, bool vsync);
    void DestroyDevice();

    void DisplayFrame();
    void PrepareDrawing();
    void DrawLayerQuad();
    void DrawDecalQuad(const Sprite& sprite);

    uint32_t CreateTexture(uint32_t width, uint32_t height);
    uint32_t DeleteTexture(uint32_t id);
    void UpdateTexture(uint32_t id, Sprite* spr);
    void UpdateTexture(uint32_t id, uint32_t w, uint32_t h, Pixel* buffer);
    void ApplyTexture(uint32_t id);

    void UpdateViewport(const vu2d& pos, const vu2d& size);
    void ClearBuffer(Pixel p, bool depth);

    Application* App;
  };

  class Platform final {
  private:

#ifdef PIXEL_LINUX

    X11::Display* pDisplay = nullptr;
    X11::Window pWindowRoot;
    X11::Window pWindow;
    X11::XVisualInfo* pVisualInfo;
    X11::Colormap pColorMap;
    X11::XSetWindowAttributes pWinAttr;

#endif

  public:
    rcode ApplicationStartUp();
    rcode ApplicationCleanUp();

    rcode ThreadStartUp();
    rcode ThreadCleanUp();

    rcode CreateGraphics(bool fullscreen, bool vsync, const vu2d& viewpos, const vu2d& viewsize);
    rcode CreateWindowPane(const vu2d& winpos, vu2d& winsize, bool fullscreen);
    rcode SetWindowTitle(const std::string& s);

    rcode HandleSystemEvent();

    Application* App;
  };

  static std::map<size_t, uint8_t> pKeyMap;

  class Application final {
  public:
    typedef rcode (*callback_t)(Application&);
    typedef struct {
      vu2d size = vu2d(256, 256);
      vu2d position = vu2d(25, 25);
      uint8_t scale = 2;

      std::string name = "Pixel";
      pixel::DrawingMode mode = DrawingMode::FULL_ALPHA;

      bool fullscreen = false;
      bool vsync = false;
      bool clearbuffer = true;
      Pixel buffercolor = Black;

      callback_t on_launch = nullptr;
      callback_t on_update = nullptr;
      callback_t on_close = nullptr;
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
    void CreateSprite(const std::string& filename);
    void CreateSprite(uint32_t w, uint32_t h);

    Sprite* GetSprite(uint32_t id);
    void UpdateSprite(uint32_t id);

  public:
    void Draw(const vu2d& pos, const Pixel& pixel);
    void DrawLine(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel);

    void DrawCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel);
    void FillCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel);

    void DrawRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel);
    void FillRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel);

    void DrawTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel);
    void FillTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel);

    void DrawSprite(const vu2d& pos, uint8_t sprite, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);
    void DrawPartialSprite(const vu2d& pos, const vu2d& spos, const vu2d& ssize, uint8_t sprite, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);

    void DrawWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& pos, const Pixel& tint = White);
    void DrawPartialWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& post, const vf2d& spos, const vf2d& ssize, const Pixel& tint = White);

    void DrawRotatedSprite(const vf2d& pos, uint8_t sprite, float alpha, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);
    void DrawPartialRotatedSprite(const vf2d& pos, uint8_t sprite, float alpha, const vf2d& spos, const vf2d& ssize, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);

  private:
    void UpdateMouse(uint32_t x, uint32_t y);
    void UpdateMouseWheel(uint32_t delta);
    void UpdateWindowSize(uint32_t x, uint32_t y);

    void UpdateViewport();
    void ConstructFontSheet();

    void UpdateMouseState(uint32_t button, bool state);
    void UpdateKeyState(uint32_t key, bool state);

    void UpdateMouseFocus(bool state);
    void UpdateKeyFocus(bool state);

  public:
    bool ShouldExist() const;
    pixel::DrawingMode DrawingMode() const;

    vu2d DrawableSize() const;
    const vu2d& ScreenSize() const;

    const vu2d& WindowSize() const;
    const vu2d& WindowPos() const;

    const vu2d& MousePos() const;
    uint32_t MouseWheel() const;

    const Button& Mouse(pixel::Mouse button) const;
    const Button& Key(Key key) const;

    float et() const;
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
    std::string pWindowName = "";
    std::string pWindowTittle = "";

  private:
    bool pVsync = false;
    bool pFullScreen = false;

    bool pClearBuffer = true;
    Pixel pBufferColor = Black;

    std::atomic<bool> pHasBeenClosed {false};
    std::atomic<bool> pShouldExist {false};

  private:
    vu2d pMousePos;
    uint32_t pMouseWheel = 0;

    Button pMouseButtons[3] = {};
    bool pMouseButtonsOld[3] = {};
    bool pMouseButtonsNew[3] = {};

    Button pKeyboardKeys[256] = {};
    bool pKeyboardKeysOld[256] = {};
    bool pKeyboardKeysNew[256] = {};

    bool pHasInputFocus = false;
    bool pHasMouseFocus = false;

  private:
    std::chrono::system_clock::time_point pClock1;
    std::chrono::system_clock::time_point pClock2;
    std::chrono::duration<float> pElapsedTimer;

    float pElapsedTime = 0.0f;
    float pFrameTimer = 0.0f;
    uint32_t pFrameCount = 0;
    uint32_t pFrameRate = 0;

  private:
    Pixel* pBuffer = nullptr;
    uint32_t pBufferId = 0xFFFFFFFF;

    std::vector<Sprite> pSprites;
    std::vector<Sprite*> pSpritesPending;
    pixel::DrawingMode pDrawingMode = pixel::DrawingMode::NO_ALPHA;

  private:
    callback_t pOnLaunch;
    callback_t pOnUpdate;
    callback_t pOnClose;

    Platform pPlatform;
    Renderer pRenderer;

  private:
    void pEngineThread();
    void pConfigureSystem();
  };
}

/*
____________________________

  Function definitions
____________________________

*/

namespace pixel {
  Pixel::Pixel() { v.r = 0; v.g = 0; v.b = 0; v.a = 0xFF; }
  Pixel::Pixel(uint32_t p) { n = p; }

  Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    n = red | (green << 8) | (blue << 16) | (alpha << 24);
  }

  bool Pixel::operator==(const Pixel& p) const { return n == p.n; }
  bool Pixel::operator!=(const Pixel& p) const { return n != p.n; }


  Sprite::Sprite(uint32_t w, uint32_t h) {
    if(pBuffer) delete[] pBuffer;

    pSize.x = w;
    pSize.y = h;

    pBuffer = new Pixel[w * h];
    for(uint32_t i = 0; i < w * h; i++) {
      pBuffer[i] = Pixel();
    }
  }

  Sprite::~Sprite() {
    if(pBuffer) delete[] pBuffer;
  }

  Sprite::Sprite(const std::string& filename) {
    if (LoadFromFile(filename) != rcode::ok) throw std::exception();
  }

  Sprite::Sprite(const Sprite& src) :
    pSize(src.pSize),
    pUvScale(src.pUvScale),
    pBufferId(src.pBufferId),
    pPos(src.pPos),
    pUv(src.pUv),
    pW{src.pW[0], src.pW[1], src.pW[2], src.pW[3]},
    pTint(src.pTint) {

    pBuffer = new Pixel[src.pSize.prod()];

    for(uint32_t i = 0; i < src.pSize.prod(); i++) {
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

  vu2d Sprite::GetSize() const {
    return pSize;
  }

  void Sprite::SetSize(uint32_t w, uint32_t h) {
    pSize = vu2d(w, h);
  }

  rcode Sprite::LoadFromFile(const std::string& filename) {
    rcode code = Helper::LoadImage(this, filename);
    return code;
  }

  rcode Sprite::SaveToFile(const std::string& filename) {
    return Helper::SaveImage(this, filename);
  }

  void Sprite::Swap(Sprite& other) noexcept {
    std::swap(pSize, other.pSize);
    std::swap(pUvScale, other.pUvScale);

    std::swap(pBuffer, other.pBuffer);
    std::swap(pBufferId, other.pBufferId);

    std::swap(pPos, other.pPos);
    std::swap(pUv, other.pUv);
    std::swap(pW, other.pW);
    std::swap(pTint, other.pTint);
  }
}

namespace pixel {
  Application::Application(Application::params_t params) {
    pConfigureSystem();

    pWindowSize = params.size * params.scale;
    pWindowPos = params.position;

    pScreenSize = params.size;
    pInvScreenSize = vf2d(1.0f / params.size.x, 1.0f / params.size.y);
    pScale = params.scale;

    pWindowName = params.name;
    pWindowTittle = params.name + " - FPS: 0";

    pDrawingMode = params.mode;

    pVsync = params.vsync;
    pFullScreen = params.fullscreen;
    pClearBuffer = params.clearbuffer;
    pBufferColor = params.buffercolor;

    pOnLaunch = params.on_launch;
    pOnUpdate = params.on_update;
    pOnClose = params.on_close;
  }

  Application::~Application() {}

  void Application::pEngineThread() {
    if (pPlatform.ThreadStartUp() == rcode::err) return;
    if (pPlatform.CreateGraphics(pFullScreen, pVsync, pViewPos, pViewSize) == rcode::err) return;

    ConstructFontSheet();

    pBuffer = new Pixel[pScreenSize.prod()];
    for (uint32_t i = 0; i < pScreenSize.prod(); i++) {
      pBuffer[i] = Pixel();
    }

    pBufferId = pRenderer.CreateTexture(pScreenSize.x, pScreenSize.y);
    pRenderer.UpdateTexture(pBufferId, pScreenSize.x, pScreenSize.y, pBuffer);

    pClock1 = std::chrono::system_clock::now();
    pClock2 = std::chrono::system_clock::now();

    if (pOnLaunch) {
      if (pOnLaunch(*this) != rcode::ok) pShouldExist = false;
    }

    while (pShouldExist) {
      while (pShouldExist){
        pClock2 = std::chrono::system_clock::now();
        pElapsedTimer = pClock2 - pClock1;
        pClock1 = pClock2;
        pElapsedTime = pElapsedTimer.count();

        pFrameTimer += pElapsedTime;
        pFrameCount++;

        pPlatform.HandleSystemEvent();

        if(pFrameTimer >= 1.0f) {
          pFrameRate = pFrameCount;
          pFrameTimer -= 1.0f;

          pWindowTittle = pWindowName + " - FPS: " + std::to_string(pFrameRate);
          pPlatform.SetWindowTitle(pWindowTittle);

          pFrameCount = 0;
        }

        for(uint32_t i = 0; i < 3; i++) {
          pMouseButtons[i].pressed = false;
          pMouseButtons[i].released = false;

          if(pMouseButtonsNew[i] != pMouseButtonsOld[i]) {
            if(pMouseButtonsNew[i]) {
              pMouseButtons[i].pressed = !pMouseButtons[i].held;
              pMouseButtons[i].held = true;

            } else {
              pMouseButtons[i].released = true;
              pMouseButtons[i].held = false;
            }
          }

          pMouseButtonsOld[i] = pMouseButtonsNew[i];
        }

        for(uint32_t i = 0; i < 256; i++) {
          pKeyboardKeys[i].pressed = false;
          pKeyboardKeys[i].released = false;

          if(pKeyboardKeysNew[i] != pKeyboardKeysOld[i]) {
            if(pKeyboardKeysNew[i]) {

              pKeyboardKeys[i].pressed = !pKeyboardKeys[i].held;
              pKeyboardKeys[i].held = true;

            } else {
              pKeyboardKeys[i].released = true;
              pKeyboardKeys[i].held = false;
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
          if (pOnUpdate(*this) != rcode::ok) pShouldExist = false;
        }

        pRenderer.UpdateViewport(pViewPos, pViewSize);
        pRenderer.ClearBuffer(Black, true);
        pRenderer.PrepareDrawing();

        pRenderer.ApplyTexture(pBufferId);
        pRenderer.UpdateTexture(pBufferId, pScreenSize.x, pScreenSize.y, pBuffer);
        pRenderer.DrawLayerQuad();

        for (auto& s : pSpritesPending) {
          pRenderer.ApplyTexture(s->pBufferId);
          pRenderer.DrawDecalQuad(*s);
        }

        pSpritesPending.clear();
        pRenderer.DisplayFrame();
      }

      if (pOnClose) {
        if (pOnClose(*this) != rcode::ok) pShouldExist = true;
      }
    }

    delete[] pBuffer;
    pRenderer.DeleteTexture(pBufferId);

    for (auto& s : pSprites) {
      if (s.pBufferId != 0xFFFFFFFF) pRenderer.DeleteTexture(s.pBufferId);
    }

    pPlatform.ThreadCleanUp();
    pPlatform.ApplicationCleanUp();

    pHasBeenClosed = true;
  }

  rcode Application::Launch(bool background) {
    if (pHasBeenClosed) return rcode::abort;

    if (pPlatform.ApplicationStartUp() != rcode::ok) return rcode::err;
    if (pPlatform.CreateWindowPane(pWindowPos, pWindowSize, pFullScreen) != rcode::ok) return rcode::err;

    UpdateViewport();

    pShouldExist = true;
    std::thread thread = std::thread(&pixel::Application::pEngineThread, this);
    
    if (background) {
      thread.detach();

    } else {
      thread.join();
    }
    
    return rcode::ok;
  }

  void Application::Close() {
    pShouldExist = false;
  }

  void Application::EnsureClosed() {
    while (!pHasBeenClosed) {}
  }

  void Application::SetName(const std::string& name) {
    pWindowName = name;
  }

  void Application::SetDrawingMode(pixel::DrawingMode mode) {
    pDrawingMode = mode;
  }

  void Application::CreateSprite(const std::string& filename) {
    pSprites.push_back(Sprite(filename));
    UpdateSprite(pSprites.size() - 1);
  }

  void Application::CreateSprite(uint32_t w, uint32_t h) {
    pSprites.push_back(Sprite(w, h));
    UpdateSprite(pSprites.size() - 1);
  }

  Sprite* Application::GetSprite(uint32_t id) {
    return &pSprites.at(id);
  }

  void Application::UpdateSprite(uint32_t id) {
    Sprite* s = &pSprites.at(id);

    if (s->pBufferId != 0xFFFFFFFF) pRenderer.DeleteTexture(s->pBufferId);

    s->pBufferId = pRenderer.CreateTexture(s->pSize.x, s->pSize.y);
    pRenderer.ApplyTexture(s->pBufferId);
    pRenderer.UpdateTexture(s->pBufferId, s);
  }

  void Application::Draw(const vu2d& pos, const Pixel& pixel) {
    if (pos.x >= pScreenSize.x) return;
    if (pos.y >= pScreenSize.y) return;

    if (pDrawingMode == DrawingMode::FULL_ALPHA) {
      Pixel d = pBuffer[pos.y * pScreenSize.x + pos.x];

      float a = (float) (pixel.v.a / 255.0f);
      float c = 1.0f - a;

      float r = a * (float) pixel.v.r + c * (float) d.v.r;
      float g = a * (float) pixel.v.g + c * (float) d.v.g;
      float b = a * (float) pixel.v.b + c * (float) d.v.b;

      pBuffer[pos.y * pScreenSize.x + pos.x].n = (uint8_t)r | ((uint8_t)g << 8) | ((uint8_t)b << 16) | (255 << 24);

    } else if (pDrawingMode == DrawingMode::NO_ALPHA) {
      pBuffer[pos.y * pScreenSize.x + pos.x] = pixel;

    } else if (pixel.v.a == 255) {
      pBuffer[pos.y * pScreenSize.x + pos.x] = pixel;
    }
  }

  void Application::DrawLine(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
    int32_t x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = pos2.x - pos1.x; dy = pos2.y - pos1.y;

    if(dx == 0) {
      if(pos2.y < pos1.y) for(y = pos2.y; y <= (int32_t) pos1.y; y++) Draw(vu2d(pos2.x, y), pixel);
      else for(y = pos1.y; y <= (int32_t) pos2.y; y++) Draw(vu2d(pos1.x, y), pixel);

      return;
    }

    if(dy == 0) {
      if(pos2.x < pos1.x) for(x = pos2.x; x <= (int32_t) pos1.x; x++) Draw(vu2d(x, pos2.y), pixel);
      for(x = pos1.x; x <= (int32_t) pos2.x; x++) Draw(vu2d(x, pos1.y), pixel);

      return;
    }

    dx1 = std::abs(dx); dy1 = std::abs(dy);
    px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;

    if(dy1 <= dx1) {
      if(dx >= 0) {
        x = pos1.x; y = pos1.y; xe = pos2.x;
      } else {
        x = pos2.x; y = pos2.y; xe = pos1.x;
      }

      Draw(vu2d(x, y), pixel);

      for(i = 0; x < xe; i++) {

        x = x + 1;

        if(px < 0)
          px = px + 2 * dy1;
        else {
          if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
          px = px + 2 * (dy1 - dx1);
        }

        Draw(vu2d(x, y), pixel);
      }

    } else {

      if(dy >= 0) {
        x = pos1.x; y = pos1.y; ye = pos2.y;
      } else {
        x = pos2.x; y = pos2.y; ye = pos1.y;
      }

      Draw(vu2d(x, y), pixel);

      for(i = 0; y < ye; i++) {

        y = y + 1;

        if(py <= 0) {
          py = py + 2 * dx1;
        } else {
          if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
          py = py + 2 * (dx1 - dy1);
        }

        Draw(vu2d(x, y), pixel);
      }
    }
  }

  void Application::DrawCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel) {
    uint32_t x0 = 0;
    uint32_t y0 = radius;
    int d = 3 - 2 * radius;

    if(!radius) return;

    while(y0 >= x0) {
      Draw(vu2d(pos.x + x0, pos.y - y0), pixel);
      Draw(vu2d(pos.x + y0, pos.y - x0), pixel);
      Draw(vu2d(pos.x + y0, pos.y + x0), pixel);
      Draw(vu2d(pos.x + x0, pos.y + y0), pixel);
      Draw(vu2d(pos.x - x0, pos.y + y0), pixel);
      Draw(vu2d(pos.x - y0, pos.y + x0), pixel);
      Draw(vu2d(pos.x - y0, pos.y - x0), pixel);
      Draw(vu2d(pos.x - x0, pos.y - y0), pixel);

      if(d < 0) d += 4 * x0++ + 6;
      else d += 4 * (x0++ - y0--) + 10;
    }
  }

  void Application::FillCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel) {
    int x0 = 0;
    int y0 = radius;
    int d = 3 - 2 * radius;

    if(!radius) return;

    auto scanline = [&] (int sx, int ex, int ny) {
      for(int i = sx; i <= ex; i++)
        Draw(vu2d(i, ny), pixel);
    };

    while(y0 >= x0) {
      scanline(pos.x - x0, pos.x + x0, pos.y - y0);
      scanline(pos.x - y0, pos.x + y0, pos.y - x0);
      scanline(pos.x - x0, pos.x + x0, pos.y + y0);
      scanline(pos.x - y0, pos.x + y0, pos.y + x0);

      if(d < 0) d += 4 * x0++ + 6;
      else d += 4 * (x0++ - y0--) + 10;
    }
  }

  void Application::DrawRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
    DrawLine(vu2d(pos1.x, pos1.y), vu2d(pos1.y, pos2.x), pixel);
    DrawLine(vu2d(pos1.y, pos2.x), vu2d(pos2.x, pos2.y), pixel);
    DrawLine(vu2d(pos2.x, pos2.y), vu2d(pos2.y, pos1.x), pixel);
    DrawLine(vu2d(pos2.y, pos1.x), vu2d(pos1.x, pos1.y), pixel);
  }

  void Application::FillRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
    for(uint32_t x = std::min(pos1.x, pos2.x); x <= std::max(pos1.x, pos2.x); x++) {
      for(uint32_t y = std::min(pos1.y, pos2.y); y <= std::max(pos1.y, pos2.y); y++) {
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
    auto drawline = [&] (int sx, int ex, int ny) {
      for(int i = sx; i <= ex; i++)
        Draw(vu2d(i, ny), pixel);
    };

    int32_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
    int32_t signx1, signx2, dx1, dy1, dx2, dy2;
    int32_t e1, e2;

    bool changed1 = false;
    bool changed2 = false;

    /*
    if(pos1.y > pos2.y) { std::swap(pos1.y, pos2.y); std::swap(pos1.x, pos2.x); }
    if(pos1.y > pos3.y) { std::swap(pos1.y, pos3.y); std::swap(pos1.x, pos3.x); }
    if(pos2.y > pos3.y) { std::swap(pos2.y, pos3.y); std::swap(pos2.x, pos3.x); }
    */

    t1x = t2x = pos1.x; y = pos1.y;
    dx1 = (int) (pos2.x - pos1.x);
    if(dx1 < 0) { dx1 = -dx1; signx1 = -1; } else signx1 = 1;
    dy1 = (int) (pos2.y - pos1.y);

    dx2 = (int) (pos3.x - pos1.x);
    if(dx2 < 0) { dx2 = -dx2; signx2 = -1; } else signx2 = 1;
    dy2 = (int) (pos3.y - pos1.y);

    if(dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
    if(dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

    e2 = (int) (dx2 >> 1);

    if(pos1.y == pos2.y) goto next;
    e1 = (int) (dx1 >> 1);

    for(int i = 0; i < dx1;) {
      t1xp = 0; t2xp = 0;
      if(t1x < t2x) { minx = t1x; maxx = t2x; } else { minx = t2x; maxx = t1x; }

      while(i < dx1) {
        i++;
        e1 += dy1;

        while(e1 >= dx1) {
          e1 -= dx1;

          if(changed1) t1xp = signx1;
          else          goto next1;
        }

        if(changed1) break;
        else t1x += signx1;
      }

      next1:

      while(1) {
        e2 += dy2;

        while(e2 >= dx2) {
          e2 -= dx2;

          if(changed2) t2xp = signx2;
          else          goto next2;
        }

        if(changed2)     break;
        else              t2x += signx2;
      }

      next2:

      if(minx > t1x) minx = t1x;
      if(minx > t2x) minx = t2x;
      if(maxx < t1x) maxx = t1x;
      if(maxx < t2x) maxx = t2x;

      drawline(minx, maxx, y);

      if(!changed1) t1x += signx1;
      t1x += t1xp;

      if(!changed2) t2x += signx2;
      t2x += t2xp;

      y += 1;
      if(y == (int32_t)pos2.y) break;
    }

    next:

    dx1 = (int) (pos3.x - pos2.x); if(dx1 < 0) { dx1 = -dx1; signx1 = -1; } else signx1 = 1;
    dy1 = (int) (pos3.y - pos2.y);

    t1x = pos2.x;

    if(dy1 > dx1) {
      std::swap(dy1, dx1);
      changed1 = true;
    } else changed1 = false;

    e1 = (int) (dx1 >> 1);

    for(int i = 0; i <= dx1; i++) {
      t1xp = 0; t2xp = 0;

      if(t1x < t2x) { minx = t1x; maxx = t2x; } else { minx = t2x; maxx = t1x; }

      while(i < dx1) {
        e1 += dy1;

        while(e1 >= dx1) {
          e1 -= dx1;
          if(changed1) { t1xp = signx1; break; } else goto next3;
        }

        if(changed1) break;
        else   	   	  t1x += signx1;
        if(i < dx1) i++;
      }

      next3:

      while(t2x != (int32_t)pos3.x) {
        e2 += dy2;

        while(e2 >= dx2) {
          e2 -= dx2;
          if(changed2) t2xp = signx2;
          else          goto next4;
        }

        if(changed2)     break;
        else              t2x += signx2;
      }

      next4:

      if(minx > t1x) minx = t1x;
      if(minx > t2x) minx = t2x;
      if(maxx < t1x) maxx = t1x;
      if(maxx < t2x) maxx = t2x;

      drawline(minx, maxx, y);

      if(!changed1) t1x += signx1;
      t1x += t1xp;

      if(!changed2) t2x += signx2;
      t2x += t2xp;

      y += 1;
      if(y > (int32_t)pos3.y) return;
    }
  }

  void Application::DrawSprite(const vu2d& pos, uint8_t sprite, const vf2d& scale, const Pixel& tint) {
    Sprite& spr = pSprites.at(sprite);

    vf2d newpos = {
      (float(pos.x) * pInvScreenSize.x) * 2.0f - 1.0f,
      ((float(pos.y) * pInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
    };

    vf2d newsize = {
      newpos.x + (2.0f * float(spr.pSize.x) * pInvScreenSize.x) * scale.x,
      newpos.y - (2.0f * float(spr.pSize.y) * pInvScreenSize.y) * scale.y
    };

    spr.pTint = tint;

    spr.pPos[0] = { newpos.x, newpos.y };
    spr.pPos[1] = { newpos.x, newsize.y };
    spr.pPos[2] = { newsize.x, newsize.y };
    spr.pPos[3] = { newsize.x, newpos.y };

    pSpritesPending.push_back(&spr);
  }

  void Application::DrawPartialSprite(const vu2d& pos, const vu2d& spos, const vu2d& ssize, uint8_t sprite, const vf2d& scale, const Pixel& tint) {
    Sprite& spr = pSprites.at(sprite);

    vf2d newpos = {
      (float(pos.x) * pInvScreenSize.x) * 2.0f - 1.0f,
      ((float(pos.y) * pInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
    };

    vf2d newsize = {
      newpos.x + (2.0f * (float) ssize.x * pInvScreenSize.x) * scale.x,
      newpos.y - (2.0f * (float) ssize.y * pInvScreenSize.y) * scale.y
    };

    spr.pTint = tint;

    spr.pPos[0] = { newpos.x, newpos.y };
    spr.pPos[1] = { newpos.x, newsize.y };
    spr.pPos[2] = { newsize.x, newsize.y };
    spr.pPos[3] = { newsize.x, newpos.y };

    vf2d uvtl = (vf2d) spos / (vf2d) spr.pSize * spr.pUvScale;
    vf2d uvbr = uvtl + ((vf2d) ssize / (vf2d) spr.pSize * spr.pUvScale);

    spr.pUv[0] = {uvtl.x, uvtl.y };
    spr.pUv[1] = {uvtl.x, uvbr.y };
    spr.pUv[2] = {uvbr.x, uvbr.y };
    spr.pUv[3] = {uvbr.x, uvtl.y };

    pSpritesPending.push_back(&spr);
  }

  // void Application::DrawWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& pos, const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  // void Application::DrawPartialWarpedSprite(uint8_t sprite, std::array<vf2d, 4>& post, const vf2d& spos, const vf2d& ssize, const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  // void Application::DrawRotatedSprite(const vf2d& pos, uint8_t sprite, float alpha, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  // void Application::DrawPartialRotatedSprite(const vf2d& pos, uint8_t sprite, float alpha, const vf2d& spos, const vf2d& ssize, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
  //   //! STUB: Implement function.
  // }

  void Application::UpdateMouse(uint32_t x, uint32_t y) {
    pHasMouseFocus = true;

    x -= pViewPos.x;
    y -= pViewPos.y;

    pMousePos.x = (((float)x / (float)(pWindowSize.x - (pViewPos.x * 2)) * (float)pScreenSize.x));
    pMousePos.y = (((float)y / (float)(pWindowSize.y - (pViewPos.y * 2)) * (float)pScreenSize.y));
  }

  void Application::UpdateMouseWheel(uint32_t delta) {
    pMouseWheel += delta;
  }

  void Application::UpdateWindowSize(uint32_t x, uint32_t y) {
    pWindowSize = vu2d(x, y);
    UpdateViewport();
  }

  void Application::UpdateViewport() {
    uint32_t ww = pScreenSize.x * pScale;
    uint32_t wh = pScreenSize.y * pScale;
    float wasp = (float) ww / (float) wh;

    pViewSize.x = (uint32_t) pWindowSize.x;
    pViewSize.y = (uint32_t) ((float) pViewSize.x / wasp);

    if(pViewSize.y > pWindowSize.y) {
      pViewSize.y = pWindowSize.y;
      pViewSize.x = (uint32_t) ((float) pViewSize.y * wasp);
    }

    pViewPos = (pWindowSize - pViewSize) / 2;
  }

  void Application::ConstructFontSheet() {
    //! STUB: Implement function.
  }

  void Application::UpdateMouseState(uint32_t button, bool state) {
    pMouseButtonsNew[button] = state;
  }

  void Application::UpdateKeyState(uint32_t key, bool state) {
    pKeyboardKeysNew[key] = state;
  }

  void Application::UpdateMouseFocus(bool state) {
    pHasMouseFocus = state;
  }

  void Application::UpdateKeyFocus(bool state) {
    pHasInputFocus = state;
  }

  bool Application::ShouldExist() const {
    return pShouldExist;
  }

  pixel::DrawingMode Application::DrawingMode() const {
    return pDrawingMode;
  }

  vu2d Application::DrawableSize() const {
    return pScreenSize - 1;
  }

  const vu2d& Application::ScreenSize() const {
    return pScreenSize;
  }

  const vu2d& Application::WindowSize() const {
    return pWindowSize;
  }

  const vu2d& Application::WindowPos() const {
    return pWindowPos;
  }

  const vu2d& Application::MousePos() const {
    return pMousePos;
  }

  uint32_t Application::MouseWheel() const {
    return pMouseWheel;
  }

  const Button& Application::Mouse(pixel::Mouse button) const {
    return pMouseButtons[(uint8_t)button];
  }

  const Button& Application::Key(pixel::Key key) const {
    return pKeyboardKeys[(uint8_t)key];
  }

  float Application::et() const {
    return pElapsedTime;
  }

  uint32_t Application::fps() const {
    return pFrameRate;
  }
}

/*
____________________________

  Renderer implementations
____________________________

*/



namespace pixel {  
  rcode Renderer::CreateDevice(std::vector<void*> params, bool fullscreen, bool vsync) {
    using namespace X11;
    IGNORE(fullscreen);

    pDisplay = (X11::Display*)(params[0]);
    pWindow = (X11::Window*)(params[1]);
    pVisualInfo = (X11::XVisualInfo*)(params[2]);

    pContext = glXCreateContext(pDisplay, pVisualInfo, nullptr, GL_TRUE);
    glXMakeCurrent(pDisplay, *pWindow, pContext);

    XWindowAttributes gwa;
    XGetWindowAttributes(pDisplay, *pWindow, &gwa);
    glViewport(0, 0, gwa.width, gwa.height);

    glSwapIntervalEXT = nullptr;
    glSwapIntervalEXT = (glSwapInterval_t*)glXGetProcAddress((unsigned char*)"glXSwapIntervalEXT");

    if (glSwapIntervalEXT != nullptr && !vsync) {
      glSwapIntervalEXT(pDisplay, *pWindow, 0);
    }

    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return pixel::ok;
  }

  void Renderer::DestroyDevice() {
    glXMakeCurrent(pDisplay, None, NULL);
    glXDestroyContext(pDisplay, pContext);
  }

  void Renderer::DisplayFrame() {
    X11::glXSwapBuffers(pDisplay, *pWindow);
  }

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

  void Renderer::DrawDecalQuad(const Sprite& sprite) {
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

  void Renderer::ApplyTexture(uint32_t id) {
    glBindTexture(GL_TEXTURE_2D, id);
  }

  void Renderer::UpdateViewport(const vu2d& pos, const vu2d& size) {
    glViewport(pos.x, pos.y, size.x, size.y);
  }

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

#ifdef PIXEL_LINUX

#include <png.h>

namespace pixel {
  void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length) {
    png_voidp a = png_get_io_ptr(pngPtr);
    ((std::istream*)a)->read((char*)data, length);
  }


  rcode Helper::LoadImage(Sprite* spr, const std::string& filename) {
    //if (std::filesystem::exists(filename)) return rcode::file_err;
    if (spr->pBuffer != nullptr) delete[] spr->pBuffer;

    png_structp png;
    png_infop info;

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png) {
      spr->SetSize(0, 0);
      spr->pBuffer = nullptr;

      return rcode::err;
    }

    info = png_create_info_struct(png);

    if (!info) {
      spr->SetSize(0, 0);
      spr->pBuffer = nullptr;

      return rcode::err;
    }

    if (setjmp(png_jmpbuf(png))) {
      spr->SetSize(0, 0);
      spr->pBuffer = nullptr;

      return rcode::err;
    }

    FILE* f = fopen(filename.c_str(), "r");
    if (!f) return rcode::file_err;

    png_init_io(png, f);
    png_read_info(png, info);

    png_byte color_type;
    png_byte bit_depth;
    png_bytep* row_pointers;

    spr->SetSize(png_get_image_width(png, info), png_get_image_height(png, info));
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)	png_set_expand_gray_1_2_4_to_8(png);
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
        png_bytep px = &(row[x * 4]);
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

  rcode Helper::SaveImage(Sprite* spr, const std::string& filename) {
    IGNORE(spr);
    IGNORE(filename);

    return rcode::ok;
  }

  rcode Platform::ApplicationStartUp() {
    return rcode::ok;
  }

  rcode Platform::ApplicationCleanUp() {
    X11::XCloseDisplay(pDisplay);
    X11::XDestroyWindow(pDisplay, pWindow);
    
    return rcode::ok;
  }

  rcode Platform::ThreadStartUp() {
    return rcode::ok;
  }

  rcode Platform::ThreadCleanUp() {
    App->pRenderer.DestroyDevice();
    return rcode::ok;
  }

  rcode Platform::CreateGraphics(bool fullscreen, bool vsync, const vu2d& viewpos, const vu2d& viewsize) {
    if(App->pRenderer.CreateDevice({pDisplay, &pWindow, pVisualInfo}, fullscreen, vsync) == rcode::ok) {
      App->pRenderer.UpdateViewport(viewpos, viewsize);
      return rcode::ok;

    } else {
      return rcode::err;
    }
  }

  rcode Platform::CreateWindowPane(const vu2d& winpos, vu2d& winsize, bool fullscreen) {
    using namespace X11;
    XInitThreads();

    pDisplay = XOpenDisplay(NULL);
    pWindowRoot = DefaultRootWindow(pDisplay);

    GLint glAttr[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    pVisualInfo = glXChooseVisual(pDisplay, 0, glAttr);
    pColorMap = XCreateColormap(pDisplay, pWindowRoot, pVisualInfo->visual, AllocNone);
    pWinAttr.colormap = pColorMap;

    pWinAttr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;

    pWindow = XCreateWindow(pDisplay, pWindowRoot, winpos.x, winpos.y, winsize.x, winsize.y, 0, pVisualInfo->depth, InputOutput, pVisualInfo->visual, CWColormap | CWEventMask, &pWinAttr);

    Atom wmDelete = XInternAtom(pDisplay, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(pDisplay, pWindow, &wmDelete, 1);

    XMapWindow(pDisplay, pWindow);
    XStoreName(pDisplay, pWindow, "Pixel Engine");

    if (fullscreen) {
      Atom wm_state;
      Atom fs;

      wm_state = XInternAtom(pDisplay, "_NET_WM_STATE", False);
      fs = XInternAtom(pDisplay, "_NET_WM_STATE_FULLSCREEN", False);

      XEvent event {0};
      event.type = ClientMessage;
      event.xclient.window = pWindow;
      event.xclient.message_type = wm_state;
      event.xclient.format = 32;
      event.xclient.data.l[0] = (fullscreen ? 1 : 0);
      event.xclient.data.l[1] = fs;
      event.xclient.data.l[2] = 0;
      event.xclient.data.l[3] = 0;

      XMapWindow(pDisplay, pWindow);
      XSendEvent(pDisplay, DefaultRootWindow(pDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
      XFlush(pDisplay);

      XWindowAttributes gwa;
      XGetWindowAttributes(pDisplay, pWindow, &gwa);

      winsize.x = gwa.width;
      winsize.y = gwa.height;
    }

    pKeyMap[0x00] = (uint8_t) Key::NONE;

    pKeyMap[0x61] = (uint8_t) Key::A; pKeyMap[0x6E] = (uint8_t) Key::N;
    pKeyMap[0x62] = (uint8_t) Key::B; pKeyMap[0x6F] = (uint8_t) Key::O;
    pKeyMap[0x63] = (uint8_t) Key::C; pKeyMap[0x70] = (uint8_t) Key::P;
    pKeyMap[0x64] = (uint8_t) Key::D; pKeyMap[0x71] = (uint8_t) Key::Q;
    pKeyMap[0x65] = (uint8_t) Key::E; pKeyMap[0x72] = (uint8_t) Key::R;
    pKeyMap[0x66] = (uint8_t) Key::F; pKeyMap[0x73] = (uint8_t) Key::S;
    pKeyMap[0x67] = (uint8_t) Key::G; pKeyMap[0x74] = (uint8_t) Key::T;
    pKeyMap[0x68] = (uint8_t) Key::H; pKeyMap[0x75] = (uint8_t) Key::U;
    pKeyMap[0x69] = (uint8_t) Key::I; pKeyMap[0x76] = (uint8_t) Key::V;
    pKeyMap[0x6A] = (uint8_t) Key::J; pKeyMap[0x77] = (uint8_t) Key::W;
    pKeyMap[0x6B] = (uint8_t) Key::K; pKeyMap[0x78] = (uint8_t) Key::X;
    pKeyMap[0x6C] = (uint8_t) Key::L; pKeyMap[0x79] = (uint8_t) Key::Y;
    pKeyMap[0x6D] = (uint8_t) Key::M; pKeyMap[0x7A] = (uint8_t) Key::Z;

    pKeyMap[XK_F1] = (uint8_t) Key::F1; pKeyMap[XK_F7] = (uint8_t) Key::F7;
    pKeyMap[XK_F2] = (uint8_t) Key::F2; pKeyMap[XK_F8] = (uint8_t) Key::F8;
    pKeyMap[XK_F3] = (uint8_t) Key::F3; pKeyMap[XK_F9] = (uint8_t) Key::F9;
    pKeyMap[XK_F4] = (uint8_t) Key::F4; pKeyMap[XK_F10] = (uint8_t) Key::F10;
    pKeyMap[XK_F5] = (uint8_t) Key::F5; pKeyMap[XK_F11] = (uint8_t) Key::F11;
    pKeyMap[XK_F6] = (uint8_t) Key::F6; pKeyMap[XK_F12] = (uint8_t) Key::F12;

    pKeyMap[XK_Down]  = (uint8_t) Key::DOWN; pKeyMap[XK_Up] = (uint8_t) Key::UP;
    pKeyMap[XK_Left]  = (uint8_t) Key::LEFT; pKeyMap[XK_KP_Enter] = (uint8_t) Key::ENTER;
    pKeyMap[XK_Right] = (uint8_t) Key::RIGHT; pKeyMap[XK_Return] = (uint8_t) Key::ENTER;

    pKeyMap[XK_BackSpace]   = (uint8_t) Key::BACK;    pKeyMap[XK_Page_Up]   = (uint8_t) Key::PGUP;
    pKeyMap[XK_Escape]      = (uint8_t) Key::ESCAPE;  pKeyMap[XK_Page_Down] = (uint8_t) Key::PGDN;
    pKeyMap[XK_Linefeed]    = (uint8_t) Key::ENTER;   pKeyMap[XK_Insert]    = (uint8_t) Key::INS;
    pKeyMap[XK_Pause]       = (uint8_t) Key::PAUSE;   pKeyMap[XK_Shift_L]   = (uint8_t) Key::SHIFT;
    pKeyMap[XK_Scroll_Lock] = (uint8_t) Key::SCROLL;  pKeyMap[XK_Shift_R]   = (uint8_t) Key::SHIFT;
    pKeyMap[XK_Tab]         = (uint8_t) Key::TAB;     pKeyMap[XK_Control_L] = (uint8_t) Key::CTRL;
    pKeyMap[XK_Delete]      = (uint8_t) Key::DEL;     pKeyMap[XK_Control_R] = (uint8_t) Key::CTRL;
    pKeyMap[XK_Home]        = (uint8_t) Key::HOME;    pKeyMap[XK_space]     = (uint8_t) Key::SPACE;
    pKeyMap[XK_End]         = (uint8_t) Key::END;     pKeyMap[XK_period]    = (uint8_t) Key::PERIOD;

    pKeyMap[XK_0] = (uint8_t) Key::K0; pKeyMap[XK_5] = (uint8_t) Key::K5;
    pKeyMap[XK_1] = (uint8_t) Key::K1; pKeyMap[XK_6] = (uint8_t) Key::K6;
    pKeyMap[XK_2] = (uint8_t) Key::K2; pKeyMap[XK_7] = (uint8_t) Key::K7;
    pKeyMap[XK_3] = (uint8_t) Key::K3; pKeyMap[XK_8] = (uint8_t) Key::K8;
    pKeyMap[XK_4] = (uint8_t) Key::K4; pKeyMap[XK_9] = (uint8_t) Key::K9;

    pKeyMap[XK_KP_0] = (uint8_t) Key::NP0; pKeyMap[XK_KP_5] = (uint8_t) Key::NP5;
    pKeyMap[XK_KP_1] = (uint8_t) Key::NP1; pKeyMap[XK_KP_6] = (uint8_t) Key::NP6;
    pKeyMap[XK_KP_2] = (uint8_t) Key::NP2; pKeyMap[XK_KP_7] = (uint8_t) Key::NP7;
    pKeyMap[XK_KP_3] = (uint8_t) Key::NP3; pKeyMap[XK_KP_8] = (uint8_t) Key::NP8;
    pKeyMap[XK_KP_4] = (uint8_t) Key::NP4; pKeyMap[XK_KP_9] = (uint8_t) Key::NP9;

    pKeyMap[XK_KP_Multiply] = (uint8_t) Key::NP_MUL;
    pKeyMap[XK_KP_Add]      = (uint8_t) Key::NP_ADD;
    pKeyMap[XK_KP_Divide]   = (uint8_t) Key::NP_DIV;
    pKeyMap[XK_KP_Subtract] = (uint8_t) Key::NP_SUB;
    pKeyMap[XK_KP_Decimal]  = (uint8_t) Key::NP_DECIMAL;

    return rcode::ok;
  }

  rcode Platform::SetWindowTitle(const std::string& s) {
    X11::XStoreName(pDisplay, pWindow, s.c_str());
    return rcode::ok;
  }

  rcode Platform::HandleSystemEvent() {
    using namespace X11;

    XEvent event;
    while (XPending(pDisplay)) {
      XNextEvent(pDisplay, &event);

      if (event.type == ClientMessage) {
        App->Close();

      } else if (event.type == FocusOut) {
        App->UpdateKeyFocus(false);

      } else if (event.type == FocusIn) {
        App->UpdateKeyFocus(true);

      } else if (event.type == MotionNotify) {
        App->UpdateMouse(event.xmotion.x, event.xmotion.y);

      } else if (event.type == ButtonPress) {
        switch (event.xbutton.button) {
        case 4:	App->UpdateMouseWheel(100); break;
        case 5:	App->UpdateMouseWheel(-100); break;

        case 1:	App->UpdateMouseState(0, true); break;
        case 2:	App->UpdateMouseState(1, true); break;
        case 3:	App->UpdateMouseState(2, true); break;

        default: break;
        }

      } else if (event.type == ButtonRelease) {
        switch (event.xbutton.button) {
        case 1:	App->UpdateMouseState(0, false); break;
        case 2:	App->UpdateMouseState(2, false); break;
        case 3:	App->UpdateMouseState(1, false); break;

        default: break;
        }

      } else if (event.type == KeyPress) {
        KeySym s = XLookupKeysym(&event.xkey, 0);
        App->UpdateKeyState(pKeyMap[s], true);

        XKeyEvent* e = (XKeyEvent*)&event;
        XLookupString(e, NULL, 0, &s, NULL);
        App->UpdateKeyState(pKeyMap[s], true);

      } else if (event.type == KeyRelease) {
        KeySym s = XLookupKeysym(&event.xkey, 0);
        App->UpdateKeyState(pKeyMap[s], false);

        XKeyEvent* e = (XKeyEvent*)&event;
        XLookupString(e, NULL, 0, &s, NULL);
        App->UpdateKeyState(pKeyMap[s], false);

      } else if (event.type == Expose) {
        XWindowAttributes gwa;

        XGetWindowAttributes(pDisplay, pWindow, &gwa);
        App->UpdateWindowSize(gwa.width, gwa.height);

      } else if (event.type == ConfigureNotify) {
        XConfigureEvent xce = event.xconfigure;
        App->UpdateWindowSize(xce.width, xce.height);
      }
    }

    return rcode::ok;
  }
}

#endif /* PIXEL_LINUX */

#ifdef PIXEL_WIN
  #error "Currently unsupported platform."
#endif /* PIXEL_WIN */

#ifdef PIXEL_MACOS
   #error "Currently unsupported platform."
#endif /* PIXEL_MACOS */

/*
____________________________

   Platform configuration
____________________________

*/

namespace pixel {
  void Application::pConfigureSystem() {
    pPlatform.App = this;
    pRenderer.App = this;
  }
}
