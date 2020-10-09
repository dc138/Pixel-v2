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
    
    Only windows build are supported at the moment, as this library makes use of
    the Win32 API. Linux support may come in the future.
    

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
		~~~~~~~~~~~~~~~~~

		The folowing program will create a wimple application window and draw 50 random
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

		    if(app.KeyboardKey(Key::ESCAPE).pressed) {
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

*/

#pragma once

#ifdef __linux__
  #define PIXEL_LINUX
#elif defined(_WIN32)
  #define PIXEL_WIN
#elif defined(__APPLE__)
  #define PIXEL_MACOS
#else
  #error "Unsupported platform."
#endif

#ifndef PIXEL_USE_DIRECTX
  #define PIXEL_USE_OPENGL
#endif

#ifdef PIXEL_WIN
  #define PIXEL_USE_GDI
#else
  #define PIXEL_USE_LIBPNG
#endif

#define PIXEL_VERSION_MAJOR 2
#define PIXEL_VERSION_MINOR 0
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


/*
___________________________
		
    Header declarations
___________________________

*/

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

	enum class DrawingMode: uint8_t {
		NO_ALPHA, FULL_ALPHA, MASK
	};

	template<class T> struct v2d {
		T x = 0; T y = 0;

		v2d(): x(0), y(0) {}
		v2d(T x, T y): x(x), y(y) {}
		v2d(const v2d& v): x(v.x), y(v.y) {}

		T prod() const { return x * y; }

		v2d operator + (const T& rhs) const { return v2d(this->x + rhs, this->y + rhs); }
		v2d operator - (const T& rhs) const { return v2d(this->x - rhs, this->y - rhs); }
		v2d operator * (const T& rhs) const { return v2d(this->x * rhs, this->y * rhs); }
		v2d operator / (const T& rhs) const { return v2d(this->x / rhs, this->y / rhs); }

		v2d operator + (const v2d& rhs) const { return v2d(this->x + rhs.x, this->y + rhs.y); }
		v2d operator - (const v2d& rhs) const { return v2d(this->x - rhs.x, this->y - rhs.y); }
		v2d operator * (const v2d& rhs) const { return v2d(this->x * rhs.x, this->y * rhs.y); }
		v2d operator / (const v2d& rhs) const { return v2d(this->x / rhs.x, this->y / rhs.y); }

		v2d& operator += (const T& rhs) { this->x += rhs; this->y += rhs; return *this; }
		v2d& operator -= (const T& rhs) { this->x -= rhs; this->y -= rhs; return *this; }
		v2d& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
		v2d& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }

		v2d& operator += (const v2d& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
		v2d& operator -= (const v2d& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
		v2d& operator *= (const v2d& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
		v2d& operator /= (const v2d& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }

		operator v2d<int32_t>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
		operator v2d<float  >() const { return { static_cast<float  >(this->x), static_cast<float  >(this->y) }; }
		operator v2d<double >() const { return { static_cast<double >(this->x), static_cast<double >(this->y) }; }
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
		friend class Renderer_OpenGL;
		friend class ImageLoader_LibPNG;

  public:
    Sprite(const Sprite& other) = delete;

	public:
		Pixel GetPixel(uint32_t x, uint32_t y) const;
		void SetPixel(uint32_t x, uint32_t y, const Pixel& p);

		vu2d GetSize() const;
		void SetSize(uint32_t w, uint32_t h);

	public:
    rcode LoadFromFile(const std::string& filename);
    rcode SaveToFile(const std::string& filename);
		void Update();

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

  class Renderer {
	public:
		virtual ~Renderer() = default;

		virtual void PrepareDevice() = 0;
		virtual rcode CreateDevice(std::vector<void*> params, bool fullscreen, bool vsync) = 0;
		virtual rcode DestroyDevice() = 0;

		virtual void DisplayFrame() = 0;
		virtual void PrepareDrawing() = 0;
		virtual void DrawLayerQuad() = 0;
		virtual void DrawDecalQuad(const Sprite& sprite) = 0;

		virtual uint32_t CreateTexture(uint32_t width, uint32_t height) = 0;
		virtual uint32_t DeleteTexture(uint32_t id) = 0;
		virtual void UpdateTexture(uint32_t id, Sprite* spr) = 0;
		virtual void UpdateTexture(uint32_t id, uint32_t w, uint32_t h, Pixel* buffer) = 0;
		virtual void ApplyTexture(uint32_t id) = 0;

		virtual void UpdateViewport(const vu2d& pos, const vu2d& size) = 0;
		virtual void ClearBuffer(Pixel p, bool depth) = 0;

    Application* App;
	};

	class Platform {
	public:
		virtual ~Platform() = default;

		virtual rcode ApplicationStartUp() = 0;
		virtual rcode ApplicationCleanUp() = 0;

		virtual rcode ThreadStartUp() = 0;
		virtual rcode ThreadCleanUp() = 0;

		virtual rcode CreateGraphics(bool fullscreen, bool vsync, const vu2d& viewpos, const vu2d& viewsize) = 0;
		virtual rcode CreateWindowPane(const vu2d& winpos, vu2d& winsize, bool fullscreen) = 0;
		virtual rcode SetWindowTitle(const std::string& s) = 0;

		virtual rcode StartSystemEventLoop() = 0;
		virtual rcode HandleSystemEvent() = 0;

		Application* App;
	};

  class ImageLoader {
  public:
    ImageLoader() = default;
		virtual ~ImageLoader() = default;

		virtual rcode LoadImage(Sprite* spr, const std::string& filename) = 0;
		virtual rcode SaveImage(Sprite* spr, const std::string& filename) = 0;
  };

	static std::unique_ptr<Renderer> renderer;
	static std::unique_ptr<Platform> platform;
  static std::unique_ptr<ImageLoader> loader;
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

			callback_t on_launch = nullptr;
			callback_t on_update = nullptr;
			callback_t on_close = nullptr;
		} params_t;

  public:
		Application(params_t params);
		~Application();

	public:
		rcode Launch();

	public:
		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;

	public:
    callback_t pOnLaunch;
    callback_t pOnUpdate;
    callback_t pOnClose;

  public:
		void Close();

		void SetName(const std::string& name);
		void SetDrawingMode(pixel::DrawingMode mode);

  public:
		void Draw(const vu2d& pos, const Pixel& pixel);
		void DrawLine(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel);

		void DrawCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel);
		void FillCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel);

		void DrawRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel);
		void FillRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel);

		void DrawTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel);
		void FillTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel);

		void DrawSprite(const vf2d& pos, Sprite* sprite, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);
		void DrawPartialSprite(const vf2d& pos, const vf2d& spos, const vf2d& ssize, Sprite* sprite, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);

		void DrawWarpedSprite(Sprite* sprite, std::array<vf2d, 4>& pos, const Pixel& tint = White);
		void DrawPartialWarpedSprite(Sprite* sprite, std::array<vf2d, 4>& post, const vf2d& spos, const vf2d& ssize, const Pixel& tint = White);

		void DrawRotatedSprite(const vf2d& pos, Sprite* sprite, float alpha, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);
		void DrawPartialRotatedSprite(const vf2d& pos, Sprite* sprite, float alpha, const vf2d& spos, const vf2d& ssize, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White);

  public:
		void UpdateMouse(int32_t x, int32_t y);
		void UpdateMouseWheel(int32_t delta);
		void UpdateWindowSize(int32_t x, int32_t y);

		void UpdateViewport();
		void ConstructFontSheet();

		void UpdateMouseState(int32_t button, bool state);
		void UpdateKeyState(int32_t key, bool state);

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

		const Button& MouseLeft() const;
		const Button& MouseRight() const;
		const Button& MouseMiddle() const;

		const Button& KeyboardKey(Key key) const;

		float et() const;
		uint32_t FPS() const;

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

		std::atomic<bool> pShouldExist {false};

	private:
		std::map<size_t, uint8_t> pKeyMap;

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
    void pEngineThread();
    void pUpdate();
    void pConfigureSystem();

  private:
		Pixel* pBuffer = nullptr;
		uint32_t pBufferId = 0xFFFFFFFF;

		std::vector<Sprite*> pSprites;
		pixel::DrawingMode pDrawingMode = pixel::DrawingMode::NO_ALPHA;
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

    Update();
  }

  Sprite::~Sprite() {
    if(pBuffer) delete[] pBuffer;
    if(pBufferId != 0xFFFFFFFF) renderer->DeleteTexture(pBufferId);
  }

  Sprite::Sprite(const std::string& filename) {
    LoadFromFile(filename);
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
    return loader->LoadImage(this, filename);
  }

  rcode Sprite::SaveToFile(const std::string& filename) {
    return loader->SaveImage(this, filename);
  }

  void Sprite::Update() {
    if(pBufferId != 0xFFFFFFFF) renderer->DeleteTexture(pBufferId);

    pBufferId = renderer->CreateTexture(this->pSize.x, this->pSize.y);
    renderer->ApplyTexture(pBufferId);
    renderer->UpdateTexture(pBufferId, this);
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

		pOnLaunch = params.on_launch;
		pOnUpdate = params.on_update;
		pOnClose = params.on_close;
	}

	Application::~Application() {}

	void Application::pEngineThread() {
		if (platform->ThreadStartUp() == rcode::err) return;
		if (platform->CreateGraphics(pFullScreen, pVsync, pViewPos, pViewSize) == rcode::err) return;

		ConstructFontSheet();

		pBuffer = new Pixel[pScreenSize.prod()];
		for(uint32_t i = 0; i < pScreenSize.prod(); i++) {
      pBuffer[i] = Pixel();
    }

		pBufferId = renderer->CreateTexture(pScreenSize.x, pScreenSize.y);
		renderer->UpdateTexture(pBufferId, pScreenSize.x, pScreenSize.y, pBuffer);

		pClock1 = std::chrono::system_clock::now();
		pClock2 = std::chrono::system_clock::now();

		if (pOnLaunch) {
			if (pOnLaunch(*this) == rcode::err) pShouldExist = false;
		}

		while (pShouldExist) {
			while (pShouldExist){
				pUpdate();
			}
			
			if (pOnClose) {
				if (pOnClose(*this) == rcode::abort) pShouldExist = true;
			}
		}
		
		platform->ThreadCleanUp();
	}

  void Application::pUpdate() {
		pClock2 = std::chrono::system_clock::now();
		pElapsedTimer = pClock2 - pClock1;
		pClock1 = pClock2;
		pElapsedTime = pElapsedTimer.count();

		pFrameTimer += pElapsedTime;
		pFrameCount++;

		platform->HandleSystemEvent();

		if(pFrameTimer >= 1.0f) {
			pFrameRate = pFrameCount;
			pFrameTimer -= 1.0f;

			pWindowTittle = pWindowName + " - FPS: " + std::to_string(pFrameRate);
			platform->SetWindowTitle(pWindowTittle);

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

		if (pOnUpdate) {
			if (pOnUpdate(*this) == rcode::abort) pShouldExist = false;
		}

		renderer->UpdateViewport(pViewPos, pViewSize);
		renderer->ClearBuffer(Black, true);
		renderer->PrepareDrawing();

		renderer->ApplyTexture(pBufferId);
		renderer->UpdateTexture(pBufferId, pScreenSize.x, pScreenSize.y, pBuffer);
		renderer->DrawLayerQuad();

		for (auto& s : pSprites) {
			renderer->ApplyTexture(s->pBufferId);
			renderer->DrawDecalQuad(*s);
		}

		renderer->DisplayFrame();
	}

	rcode Application::Launch() {
		if (platform->ApplicationStartUp() != rcode::ok) return rcode::err;
		if (platform->CreateWindowPane(pWindowPos, pWindowSize, pFullScreen) != rcode::ok) return rcode::err;

		UpdateViewport();
		
		pShouldExist = true;
		std::thread thread = std::thread(&pixel::Application::pEngineThread, this);

		platform->StartSystemEventLoop();

		thread.join();

		if (platform->ApplicationCleanUp() != rcode::ok) return rcode::err;
		return rcode::ok;
	}

	void Application::Close() {
		pShouldExist = false;
	}

	void Application::SetName(const std::string& name) {
		pWindowName = name;
	}

	void Application::SetDrawingMode(pixel::DrawingMode mode) {
		pDrawingMode = mode;
	}

	void Application::Draw(const vu2d& pos, const Pixel& pixel) {
		if((pos.y * pScreenSize.x + pos.x) > pScreenSize.prod()) return;

		if(pos.x >= pScreenSize.x) return;
		if(pos.y >= pScreenSize.y) return;

		if(pDrawingMode == DrawingMode::FULL_ALPHA) {
			Pixel d = pBuffer[pos.y * pScreenSize.x + pos.x];

			float a = (float) (pixel.v.a / 255.0f);
			float c = 1.0f - a;

			float r = a * (float) pixel.v.r + c * (float) d.v.r;
			float g = a * (float) pixel.v.g + c * (float) d.v.g;
			float b = a * (float) pixel.v.b + c * (float) d.v.b;

			pBuffer[pos.y * pScreenSize.x + pos.x] = Pixel((uint8_t) r, (uint8_t) g, (uint8_t) b);

		} else if(pDrawingMode == DrawingMode::NO_ALPHA) {
			pBuffer[pos.y * pScreenSize.x + pos.x] = pixel;

		} else if(pixel.v.a == 255) {
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

	/*void Application::DrawCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel) {
		//! STUB: Implement function.
	}

	void Application::FillCircle(const vu2d& pos, uint32_t radius, const Pixel& pixel) {
		//! STUB: Implement function.
	}

	void Application::DrawRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
		//! STUB: Implement function.
	}

	void Application::FillRect(const vu2d& pos1, const vu2d& pos2, const Pixel& pixel) {
		//! STUB: Implement function.
	}

	void Application::DrawTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel) {
		//! STUB: Implement function.
	}

	void Application::FillTriangle(const vu2d& pos1, const vu2d& pos2, const vu2d& pos3, const Pixel& pixel) {
		//! STUB: Implement function.
	}

	void Application::DrawSprite(const vf2d& pos, Sprite* sprite, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
		//! STUB: Implement function.
	}

	void Application::DrawPartialSprite(const vf2d& pos, const vf2d& spos, const vf2d& ssize, Sprite* sprite, const vf2d& scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
		//! STUB: Implement function.
	}

	void Application::DrawWarpedSprite(Sprite* sprite, std::array<vf2d, 4>& pos, const Pixel& tint = White) {
		//! STUB: Implement function.
	}

	void Application::DrawPartialWarpedSprite(Sprite* sprite, std::array<vf2d, 4>& post, const vf2d& spos, const vf2d& ssize, const Pixel& tint = White) {
		//! STUB: Implement function.
	}

	void Application::DrawRotatedSprite(const vf2d& pos, Sprite* sprite, float alpha, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
		//! STUB: Implement function.
	}

	void Application::DrawPartialRotatedSprite(const vf2d& pos, Sprite* sprite, float alpha, const vf2d& spos, const vf2d& ssize, const vf2d& center = vf2d(0.0f, 0.0f), const vf2d scale = vf2d(1.0f, 1.0f), const Pixel& tint = White) {
		//! STUB: Implement function.
	}*/

	void Application::UpdateMouse(int32_t x, int32_t y) {
		pHasMouseFocus = true;

		x -= pViewPos.x;
		y -= pViewPos.y;

		pMousePos.x = (((float)x / (float)(pWindowSize.x - (pViewPos.x * 2)) * (float)pScreenSize.x));
		pMousePos.y = (((float)y / (float)(pWindowSize.y - (pViewPos.y * 2)) * (float)pScreenSize.y));

		if (pMousePos.x >= pScreenSize.x)	pMousePos.x = pScreenSize.x - 1;
		if (pMousePos.y >= pScreenSize.y)	pMousePos.y = pScreenSize.y - 1;
	}

	void Application::UpdateMouseWheel(int32_t delta) {
		pMouseWheel += delta;
	}

	void Application::UpdateWindowSize(int32_t x, int32_t y) {
		pWindowSize = vu2d(x, y);
		UpdateViewport();
	}

	void Application::UpdateViewport() {
		int32_t ww = pScreenSize.x * pScale;
		int32_t wh = pScreenSize.y * pScale;
		float wasp = (float) ww / (float) wh;

		pViewSize.x = (int32_t) pWindowSize.x;
		pViewSize.y = (int32_t) ((float) pViewSize.x / wasp);

		if(pViewSize.y > pWindowSize.y) {
			pViewSize.y = pWindowSize.y;
			pViewSize.x = (int32_t) ((float) pViewSize.y * wasp);
		}

		pViewPos = (pWindowSize - pViewSize) / 2;
	}

	void Application::ConstructFontSheet() {
		//! STUB: Implement function.
	}

	void Application::UpdateMouseState(int32_t button, bool state) {
		pMouseButtonsNew[button] = state;
	}

	void Application::UpdateKeyState(int32_t key, bool state) {
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

	const Button& Application::MouseLeft() const {
		return pMouseButtons[0];
	}

	const Button& Application::MouseRight() const {
		return pMouseButtons[1];
	}

	const Button& Application::MouseMiddle() const {
		return pMouseButtons[2];
	}

	const Button& Application::KeyboardKey(Key key) const {
		return pKeyboardKeys[(uint8_t)key];
	}

	float Application::et() const {
		return pElapsedTime;
	}

	uint32_t Application::FPS() const {
		return pFrameRate;
	}
} 


/*
____________________________

    Image Implamentations
____________________________

*/

#ifdef PIXEL_USE_GDI
  #error "Currently unsupported image loader."
#endif /* PIXEL_USE_GDI*/

#ifdef PIXEL_USE_LIBPNG
#include <png.h>

namespace pixel {
	void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length) {
		png_voidp a = png_get_io_ptr(pngPtr);
		((std::istream*)a)->read((char*)data, length);
	}

  class ImageLoader_LibPNG : public ImageLoader {
		virtual rcode LoadImage(Sprite* spr, const std::string& filename) {
			if (std::filesystem::exists(filename)) return rcode::file_err;
			if (spr->pBuffer != nullptr) delete[] spr->pBuffer;

			png_structp png;
			png_infop info;

			auto loadPNG = [&]() {
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
			};

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

			FILE* f = fopen(filename.c_str(), "rb");
			if (!f) return rcode::file_err;

			png_init_io(png, f);
			loadPNG();
			fclose(f);

			return rcode::ok;
		}

		virtual rcode SaveImage(Sprite* spr, const std::string& filename) {
			IGNORE(spr);
			IGNORE(filename);

			return rcode::ok;
		}
  };
}
#endif /* PIXEL_USE_LIBPNG */

/*
____________________________

  Renderer implementations
____________________________

*/

#ifdef PIXEL_USE_DIRECTX
  #error "Currently unsupported renderer."
#endif /* PIXEL_USE_DIRECTX */

#ifdef PIXEL_USE_OPENGL
  #ifdef PIXEL_LINUX
  	#include <GL/gl.h>

    namespace X11 {
      #include <GL/glx.h>
      #include <X11/X.h>
      #include <X11/Xlib.h>
    }
  
  	typedef int(glSwapInterval_t)(X11::Display* dpy, X11::GLXDrawable drawable, int interval);
  	static glSwapInterval_t* glSwapIntervalEXT;

  	typedef X11::GLXContext glDeviceContext_t;
  	typedef X11::GLXContext glRenderContext_t;
  #elif defined(PIXEL_WIN)
  #elif defined(PIXEL_MACOS)
  #endif
#endif /* PIXEL_USE_OPENGL */

namespace pixel {
  class Renderer_OpenGL : public Renderer {
	private:
		glDeviceContext_t glDeviceContext = 0;
		glRenderContext_t glRenderContext = 0;

		bool vsync = false;

		X11::Display* pDisplay = nullptr;
		X11::Window* pWindow = nullptr;
		X11::XVisualInfo* pVisualInfo = nullptr;

		virtual void PrepareDevice() override {}

		virtual rcode CreateDevice(std::vector<void*> params, bool fullscreen, bool vsync) override {
			using namespace X11;
			IGNORE(fullscreen);

			pDisplay = (X11::Display*)(params[0]);
			pWindow = (X11::Window*)(params[1]);
			pVisualInfo = (X11::XVisualInfo*)(params[2]);

			glDeviceContext = glXCreateContext(pDisplay, pVisualInfo, nullptr, GL_TRUE);
			glXMakeCurrent(pDisplay, *pWindow, glDeviceContext);

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

			return rcode::ok;
		}

		virtual rcode DestroyDevice() override {
			glXMakeCurrent(pDisplay, None, NULL);
			glXDestroyContext(pDisplay, glDeviceContext);

			return rcode::ok;
		}

		virtual void DisplayFrame() override {
			X11::glXSwapBuffers(pDisplay, *pWindow);
		}

		virtual void PrepareDrawing() override {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		virtual void DrawLayerQuad() override {
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
		
		virtual void DrawDecalQuad(const Sprite& sprite) override {
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

		virtual uint32_t CreateTexture(uint32_t width, uint32_t height) override {
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

		virtual uint32_t DeleteTexture(uint32_t id) override {
			glDeleteTextures(1, &id);
			return id;
		}

		virtual void UpdateTexture(uint32_t id, Sprite* spr) override {
			IGNORE(id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr->pSize.x, spr->pSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, spr->pBuffer);
		}

		virtual void UpdateTexture(uint32_t id, uint32_t w, uint32_t h, Pixel* buffer) override {
			IGNORE(id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		}

		virtual void ApplyTexture(uint32_t id) override {
			glBindTexture(GL_TEXTURE_2D, id);
		}

		virtual void UpdateViewport(const vu2d& pos, const vu2d& size) override {
			glViewport(pos.x, pos.y, size.x, size.y);
		}

		virtual void ClearBuffer(Pixel p, bool depth) override {
			glClearColor(float(p.v.r) / 255.0f, float(p.v.g) / 255.0f, float(p.v.b) / 255.0f, float(p.v.a) / 255.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			if (depth) glClear(GL_DEPTH_BUFFER_BIT);
		}
  };
}

/*
____________________________

  Platform implementations
____________________________

*/

#ifdef PIXEL_LINUX

namespace pixel {
  class Platform_Linux : public Platform {
  private:
		X11::Display* pDisplay = nullptr;
		X11::Window pWindowRoot;
		X11::Window pWindow;
		X11::XVisualInfo* pVisualInfo;
		X11::Colormap pColorMap;
		X11::XSetWindowAttributes pWinAttr;

  public:
    virtual rcode ApplicationStartUp() override {
      return rcode::ok;
    }

		virtual rcode ApplicationCleanUp() override {
      return rcode::ok;
    }

		virtual rcode ThreadStartUp() override {
      return rcode::ok;
    }

		virtual rcode ThreadCleanUp() override {
      renderer->DestroyDevice();
      return rcode::ok;
    }

		virtual rcode CreateGraphics(bool fullscreen, bool vsync, const vu2d& viewpos, const vu2d& viewsize) override {
      if(renderer->CreateDevice({pDisplay, &pWindow, pVisualInfo}, fullscreen, vsync) == rcode::ok) {
        renderer->UpdateViewport(viewpos, viewsize);
				return rcode::ok;

      } else {
        return rcode::err;
      }
    }

		virtual rcode CreateWindowPane(const vu2d& winpos, vu2d& winsize, bool fullscreen) override {
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
			
			pKeyMap[XK_Down] = (uint8_t) Key::DOWN; pKeyMap[XK_Up] = (uint8_t) Key::UP;
			pKeyMap[XK_Left] = (uint8_t) Key::LEFT; pKeyMap[XK_KP_Enter] = (uint8_t) Key::ENTER;
			pKeyMap[XK_Right] = (uint8_t) Key::RIGHT; pKeyMap[XK_Return] = (uint8_t) Key::ENTER;
			
			pKeyMap[XK_BackSpace] = (uint8_t) Key::BACK;  pKeyMap[XK_Page_Up] = (uint8_t) Key::PGUP;
			pKeyMap[XK_Escape] = (uint8_t) Key::ESCAPE; pKeyMap[XK_Page_Down] = (uint8_t) Key::PGDN;	
			pKeyMap[XK_Linefeed] = (uint8_t) Key::ENTER; pKeyMap[XK_Insert] = (uint8_t) Key::INS;
			pKeyMap[XK_Pause] = (uint8_t) Key::PAUSE; pKeyMap[XK_Shift_L] = (uint8_t) Key::SHIFT;
			pKeyMap[XK_Scroll_Lock] = (uint8_t) Key::SCROLL; pKeyMap[XK_Shift_R] = (uint8_t) Key::SHIFT; 
			pKeyMap[XK_Tab] = (uint8_t) Key::TAB; pKeyMap[XK_Control_L] = (uint8_t) Key::CTRL; 
			pKeyMap[XK_Delete] = (uint8_t) Key::DEL; pKeyMap[XK_Control_R] = (uint8_t) Key::CTRL;
			pKeyMap[XK_Home] = (uint8_t) Key::HOME; pKeyMap[XK_space] = (uint8_t) Key::SPACE;
			pKeyMap[XK_End] = (uint8_t) Key::END;  pKeyMap[XK_period] = (uint8_t) Key::PERIOD;

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
			pKeyMap[XK_KP_Add] = (uint8_t) Key::NP_ADD; 
			pKeyMap[XK_KP_Divide] = (uint8_t) Key::NP_DIV; 
			pKeyMap[XK_KP_Subtract] = (uint8_t) Key::NP_SUB;
			pKeyMap[XK_KP_Decimal] = (uint8_t) Key::NP_DECIMAL;

      return rcode::ok;
    }

		virtual rcode SetWindowTitle(const std::string& s) override {
      X11::XStoreName(pDisplay, pWindow, s.c_str());
			return rcode::ok;
    }

		virtual rcode StartSystemEventLoop() override {
			return rcode::ok;
    }

		virtual rcode HandleSystemEvent() override { 
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
					case 2:	App->UpdateMouseState(2, true); break;
					case 3:	App->UpdateMouseState(1, true); break;

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
  };
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

#ifdef PIXEL_USE_GDI
    loader = std::make_unique<ImageLoader_GDI>();
#endif

#ifdef PIXEL_USE_LIBPNG
    loader = std::make_unique<ImageLoader_LibPNG>();
#endif


#ifdef PIXEL_USE_DIRECTX
    renderer = std::make_unique<Renderer_DirectX>();
#endif

#ifdef PIXEL_USE_OPENGL
    renderer = std::make_unique<Renderer_OpenGL>();
#endif


#ifdef PIXEL_LINUX
    platform = std::make_unique<Platform_Linux>();
#endif

#ifdef PIXEL_WIN
    platform = std::make_unique<Platform_Win>();
#endif

#ifdef PIXEL_MACOS
    platform = std::make_unique<Platform_MacOS>();
#endif

    platform->App = this;
    renderer->App = this;
  }
}
