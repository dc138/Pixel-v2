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

/*
___________________________
		
    Header declarations
___________________________

*/

namespace pixel {

  enum rcode { ok = 0, err = 1, file_err = 2 };
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

  public:
    Sprite(const Sprite& other) = delete;

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
		virtual void DrawDecalQuad(const Sprite& decal) = 0;

		virtual uint32_t CreateTexture(uint32_t width, uint32_t height) = 0;
		virtual uint32_t DeleteTexture(uint32_t id) = 0;
		virtual void UpdateTexture(uint32_t id, Sprite* spr) = 0;
		virtual void ApplyTexture(uint32_t id) = 0;

		virtual void UpdateViewport(const vi2d& pos, const vi2d& size) = 0;
		virtual void ClearBuffer(Pixel p, bool depth) = 0;

    static Application* App;
	};

	class Platform {
	public:
		virtual ~Platform() = default;

		virtual rcode ApplicationStartUp() = 0;
		virtual rcode ApplicationCleanUp() = 0;

		virtual rcode ThreadStartUp() = 0;
		virtual rcode ThreadCleanUp() = 0;

		virtual rcode CreateGraphics(bool fullscreen, bool vsync, const vi2d& viewpos, const vi2d& viewsize) = 0;
		virtual rcode CreateWindowPane(const vi2d& winpos, vi2d& winsize, bool fullscreen) = 0;
		virtual rcode SetWindowTitle(const std::string& s) = 0;

		virtual rcode StartSystemEventLoop() = 0;
		virtual rcode HandleSystemEvent() = 0;

		static Application* App;
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
    typedef rcode (*callback_function_t)(const Application&);

  public:
		Application(const vu2d& size, uint8_t scale, const vu2d& position, const std::string& name, DrawingMode mode = DrawingMode::NO_ALPHA, bool fullScreen = false, bool vsync = false, bool launch_thread = false);
		~Application();

	public:
		void Launch();

	public:
		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;

	public:
    callback_function_t OnLaunch;
    callback_function_t OnUpdate;
    callback_function_t OnClose;

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
  class ImageLoader_LibPNG : public ImageLoader {

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

		virtual void PrepareDevice() override {
			//! STUB: To be implemented!
		}

		virtual rcode CreateDevice(std::vector<void*> params, bool fullscreen, bool vsync) override {
			using namespace X11;

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
		
		virtual void DrawDecalQuad(const Sprite& decal) override {
			glBindTexture(GL_TEXTURE_2D, decal.pBufferId);
			glBegin(GL_QUADS);
	
			glColor4ub(decal.pTint.v.r, decal.pTint.v.g, decal.pTint.v.b, decal.pTint.v.a);

			glTexCoord4f(decal.pUv[0].x, decal.pUv[0].y, 0.0f, decal.pW[0]); 
			glVertex2f(decal.pPos[0].x, decal.pPos[0].y);
			glTexCoord4f(decal.pUv[1].x, decal.pUv[1].y, 0.0f, decal.pW[1]); 
			glVertex2f(decal.pPos[1].x, decal.pPos[1].y);
			glTexCoord4f(decal.pUv[2].x, decal.pUv[2].y, 0.0f, decal.pW[2]); 
			glVertex2f(decal.pPos[2].x, decal.pPos[2].y);
			glTexCoord4f(decal.pUv[3].x, decal.pUv[3].y, 0.0f, decal.pW[3]); 
			glVertex2f(decal.pPos[3].x, decal.pPos[3].y);
			
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

		virtual void ApplyTexture(uint32_t id) override {
			glBindTexture(GL_TEXTURE_2D, id);
		}

		virtual void UpdateViewport(const vi2d& pos, const vi2d& size) override {
			glViewport(pos.x, pos.y, size.x, size.y);
		}

		virtual void ClearBuffer(Pixel p, bool depth) override {
			glClearColor(float(p.r) / 255.0f, float(p.g) / 255.0f, float(p.b) / 255.0f, float(p.a) / 255.0f);
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

		virtual rcode CreateGraphics(bool fullscreen, bool vsync, const vi2d& viewpos, const vi2d& viewsize) override {
      if(renderer->CreateDevice({pDisplay, &pWindow, pVisualInfo}, fullscreen, vsync) == rcode::ok) {
        renderer->UpdateViewport(viewpos, viewsize);

      } else {
        return rcode::err;
      }
    }

		virtual rcode CreateWindowPane(const vi2d& winpos, vi2d& winsize, bool fullscreen) override {
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

				XEvent xev{ 0 };
				xev.type = ClientMessage;
				xev.xclient.window = pWindow;
				xev.xclient.message_type = wm_state;
				xev.xclient.format = 32;
				xev.xclient.data.l[0] = (fullscreen ? 1 : 0);
				xev.xclient.data.l[1] = fs;
				xev.xclient.data.l[2] = 0;
				xev.xclient.data.l[3] = 0;                    

				XMapWindow(pDisplay, pWindow);
				XSendEvent(pDisplay, DefaultRootWindow(pDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
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

				 //! STUB: Implement event handling!
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
