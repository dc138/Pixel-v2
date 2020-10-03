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

*/

#pragma once

#ifdef __linux__
  #define PIXEL_LINUX
#elif defined(_WIN32)
  #define PIXEL_WIN
  #error "Platform support comming in the future."
#elif defined(__APPLE__)
  #define PIXEL_MACOS
  #error "Platform support comming in the future."
#else
  #error "Unsupported platform."
#endif

#define PIXEL_VERSION_MAJOR 2
#define PIXEL_VERSION_MINOR 0
#define PIXEL_VERSION_PATCH 0

#define UNUSED(x) (void(x))
#define IGNORE_BAD_CALL(expr) (try(expr;)catch(std::bad_function_call e){})

#include <string>
#include <chrono>
#include <map>
#include <algorithm>
#include <istream>
#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <filesystem>
#include <functional>
#include <version>

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
#endif

/*
___________________________
		
    Header declarations
___________________________

*/

namespace pixel {
  struct Button {
		bool pressed = false;
		bool held = false;
		bool released = false;
	};

	enum class Key: uint8_t {
		NONE,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
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
			struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
		};

		Pixel();
		Pixel(uint32_t p);
		Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);

		bool operator==(const Pixel& p) const {
			return n == p.n;
		}
		bool operator!=(const Pixel& p) const {
			return n != p.n;
		}
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

  class Sprite {
	public:
		Sprite(const std::string& filename);
		~Sprite();

		friend class Application;

	public:
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

  enum rcode { OK = 0, FAIL = 1, NO_FILE = 2 };

  class Renderer {
	public:
		virtual ~Renderer() = default;
		virtual void PrepareDevice() = 0;

		virtual rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC) = 0;
		virtual rcode DestroyDevice() = 0;

		virtual void DisplayFrame() = 0;
		virtual void PrepareDrawing() = 0;
		virtual void DrawLayerQuad(const vf2d& offset, const vf2d& scale, const Pixel tint) = 0;
		virtual void DrawDecalQuad(const Sprite& decal) = 0;

		virtual uint32_t CreateTexture(const uint32_t width, const uint32_t height) = 0;
		virtual uint32_t DeleteTexture(const uint32_t id) = 0;
		virtual void UpdateTexture(uint32_t id, Sprite* spr) = 0;
		virtual void ApplyTexture(uint32_t id) = 0;

		virtual void UpdateViewport(const vi2d& pos, const vi2d& size) = 0;
		virtual void ClearBuffer(Pixel p, bool bDepth) = 0;

    static Application* App;
	};

	class Platform {
	public:
		virtual ~Platform() = default;

		virtual rcode ApplicationStartUp() = 0;
		virtual rcode ApplicationCleanUp() = 0;

		virtual rcode ThreadStartUp() = 0;
		virtual rcode ThreadCleanUp() = 0;

		virtual rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const vi2d& vViewPos, const vi2d& vViewSize) = 0;
		virtual rcode CreateWindowPane(const vi2d& vWindowPos, vi2d& vWindowSize, bool bFullScreen) = 0;
		virtual rcode SetWindowTitle(const std::string& s) = 0;

		virtual rcode StartSystemEventLoop() = 0;
		virtual rcode HandleSystemEvent() = 0;

		static Application* App;
	};

	static std::unique_ptr<Renderer> renderer;
	static std::unique_ptr<Platform> platform;
	static std::map<size_t, uint8_t> mapKeys;

  class Application {
  public:
		Application();
		virtual ~Application();

	public:
		void Start(const vu2d& size, uint8_t scale, const vu2d& position, const std::string& name, DrawingMode mode = DrawingMode::NO_ALPHA, bool fullScreen = false, bool vsync = false, bool launch_thread = false);

	public:
		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;

	public:
		std::function<bool(const Application&)> OnCreate;
		std::function<bool(const Application&, float)> OnUpdate;

  protected:
		void Close();

		void SetName(const std::string& name);
		void SetDrawingMode(pixel::DrawingMode mode);

  protected:
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

  protected:
		bool ShouldExist() const;
		pixel::DrawingMode DrawingMode() const;

		const vu2d& DrawableSize() const;
		const vu2d& ScreenSize() const;

		const vu2d& WindowSize() const;
		const vu2d& WindowPos() const;

		const vu2d& MousePos() const;
		uint32_t MouseWheel() const;

		const Button& MouseLeft() const;
		const Button& MouseRight() const;
		const Button& MouseMiddle() const;

		const Button& KeyboardKey(Key key) const;

		float ElapsedTime() const;
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

		static std::atomic_bool pShouldExist;

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

}
