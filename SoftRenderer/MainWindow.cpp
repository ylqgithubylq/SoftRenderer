#include "Header.hpp"
#include "MainWindow.hpp"
#include "InputManager.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

namespace X
{
	struct MainWindow::HideWindows_
	{
		//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
		//
		//  PURPOSE:  Processes messages for the main window.
		//
		//  WM_COMMAND  - process the application menu
		//  WM_PAINT    - Paint the main window
		//  WM_DESTROY  - post a quit message and return
		//
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MainWindow* thiz = reinterpret_cast<MainWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

			if (thiz && thiz->hideWindows_->hWnd_)
			{
				assert(hWnd == thiz->hideWindows_->hWnd_);
				return thiz->hideWindows_->InstanceWndProc(hWnd, message, wParam, lParam);
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}

		LRESULT InstanceWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
				// Add all windows message handling here
			case WM_KEYDOWN:
			{
				window_.OnKeyDown(wParam);
				break;
			}

			case WM_KEYUP:
			{
				window_.OnKeyUp(wParam);
				break;
			}
			case WM_LBUTTONDOWN:
			{
				wParam = VK_LBUTTON;
				window_.OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
			}
				break;
			case WM_RBUTTONDOWN:
			{
				wParam = VK_RBUTTON;
				window_.OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
			}
				break;
			case WM_MBUTTONDOWN:
			{
				wParam = VK_MBUTTON;
				window_.OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
			}
				break;
			case WM_LBUTTONUP:
			{
				wParam = VK_LBUTTON;
				window_.OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
			}
				break;
			case WM_RBUTTONUP:
			{
				wParam = VK_RBUTTON;
				window_.OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
			}
				break;
			case WM_MBUTTONUP:
			{
				wParam = VK_MBUTTON;
				window_.OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
			}
				break;

			case WM_MOUSEWHEEL:
			{
				// wParam buttons should not be relied on
				window_.OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), LOWORD(lParam), HIWORD(lParam), GET_WHEEL_DELTA_WPARAM(wParam));
			}
				break;

			case WM_MOUSEMOVE:
			{
				// wParam buttons should not be relied on
				window_.OnMouseMove(GET_KEYSTATE_WPARAM(wParam), LOWORD(lParam), HIWORD(lParam));
			}
				break;

			case WM_MOUSELEAVE:
			{

			}
				break;

			case WM_ACTIVATE:
			{
				if (!HIWORD(wParam))
				{
					window_.active_ = true;
				}
				else
				{
					window_.active_ = false;
				}
				break;
			}

			case WM_SIZE:
			{
			}
				break;


			case WM_ERASEBKGND:
			{
				return 1;
			}
				break;
			case WM_CLOSE:
			{
				::PostQuitMessage(0);
			}
				break;

			default:
			{
			}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		HideWindows_(MainWindow& window)
			: window_(window)
		{
		}

		MainWindow& window_;
		HWND hWnd_;
		HDC hDC_;

		HDC hDCMem_;
		HBITMAP hBMMem_;
		u8* bitmapBits_;
		u32 lineStride_;
	};

	MainWindow::MainWindow(Setting const& setting)
		: name_(setting.title), size_(0, 0),
		active_(false), running_(true), rendering_(true)
	{
		hideWindows_ = std::make_unique<HideWindows_>(*this);

		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = HideWindows_::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = name_.c_str();
		wcex.hIconSm = nullptr;

		::RegisterClassEx(&wcex);

		clientRegionLeft_ = setting.left;
		clientRegionTop_ = setting.top;

		RECT windowRect;
		windowRect.left = clientRegionLeft_;
		windowRect.top = clientRegionTop_;
		windowRect.right = clientRegionLeft_ + setting.width;
		windowRect.bottom = clientRegionTop_ + setting.height;

		DWORD style;
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		::AdjustWindowRect(&windowRect, style, false);
		windowLeft_ = windowRect.left;
		windowTop_ = windowRect.top;

		hideWindows_->hWnd_ = ::CreateWindow(name_.c_str(), name_.c_str(), style,
			windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

		RECT clientRect;
		::GetClientRect(hideWindows_->hWnd_, &clientRect);
		u32 width = clientRect.right - clientRect.left;
		u32 height = clientRect.bottom - clientRect.top;

		::SetWindowLongPtr(hideWindows_->hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		::SetWindowLongPtr(hideWindows_->hWnd_, GWL_STYLE, style);

		::ShowWindow(hideWindows_->hWnd_, SW_SHOWNORMAL);
		::UpdateWindow(hideWindows_->hWnd_);

		windowRect.left = clientRegionLeft_;
		windowRect.top = clientRegionTop_;
		windowRect.right = clientRegionLeft_ + width;
		windowRect.bottom = clientRegionTop_ + height;
		::AdjustWindowRect(&windowRect, style, false);

		::SetWindowPos(hideWindows_->hWnd_, nullptr, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_SHOWWINDOW | SWP_NOZORDER);

		::GetClientRect(hideWindows_->hWnd_, &clientRect);
		width = clientRect.right - clientRect.left;
		height = clientRect.bottom - clientRect.top;

		size_ = Size<u32, 2>(width, height);

		hideWindows_->hDC_ = ::GetDC(hideWindows_->hWnd_);

		hideWindows_->hDCMem_ = ::CreateCompatibleDC(hideWindows_->hDC_);
		::SetBkMode(hideWindows_->hDCMem_, OPAQUE);

		BITMAPINFOHEADER bitmapInfo;
		bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.biWidth = width;
		bitmapInfo.biHeight = height;
		bitmapInfo.biPlanes = 1;
		bitmapInfo.biBitCount = 24;
		bitmapInfo.biCompression = BI_RGB;
		bitmapInfo.biSizeImage = 0;
		bitmapInfo.biXPelsPerMeter = 0;
		bitmapInfo.biYPelsPerMeter = 0;
		bitmapInfo.biClrUsed = 0;
		bitmapInfo.biClrImportant = 0;

		hideWindows_->hBMMem_ = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS, reinterpret_cast<void**>(&hideWindows_->bitmapBits_), NULL, NULL);
		// copy from MSDN
		hideWindows_->lineStride_ = ((((bitmapInfo.biWidth * bitmapInfo.biBitCount) + 31) & ~31) >> 3);

		::SelectObject(hideWindows_->hDCMem_, hideWindows_->hBMMem_);
	}


	MainWindow::~MainWindow()
	{
		if (hideWindows_->hWnd_ != nullptr)
		{
			if (hideWindows_->hDC_ != nullptr)
			{
				::ReleaseDC(hideWindows_->hWnd_, hideWindows_->hDC_);
				hideWindows_->hDC_ = nullptr;
			}
			::DeleteObject(hideWindows_->hBMMem_);
			::DeleteDC(hideWindows_->hDCMem_);

			::SetWindowLongPtr(hideWindows_->hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
			::DestroyWindow(hideWindows_->hWnd_);
			hideWindows_->hWnd_ = nullptr;
		}
	}

	bool MainWindow::HandleMessage()
	{
		MSG msg;
		BOOL hasMessage;

		do 
		{
			if (active_ && rendering_)
			{
				hasMessage = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
			}
			else
			{
				hasMessage = ::GetMessage(&msg, nullptr, 0, 0);
			}

			if (hasMessage)
			{
				if (msg.message == WM_QUIT)
				{
					running_ = false;
				}
				else
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
		}
		while (hasMessage);

		return running_;
	}

	namespace
	{
		struct RGBStruct
		{
			u8 b;
			u8 g;
			u8 r;
			RGBStruct(f32V3 const& rgb)
			{
				r = static_cast<u8>(Clamp(rgb.X(), 0.f, 1.f) * 255);
				g = static_cast<u8>(Clamp(rgb.Y(), 0.f, 1.f) * 255);
				b = static_cast<u8>(Clamp(rgb.Z(), 0.f, 1.f) * 255);
			}
		};
		static_assert(sizeof(RGBStruct) == 3, "");

	}

	void MainWindow::DrawColorRectangle(f32V3 const* rgbArray, u32 width, u32 height)
	{
		assert(width == size_.X());
		assert(height == size_.Y());

		//Draw into hDCMem
		for (u32 y = 0; y < height; ++y)
		{
			for (u32 x = 0; x < width; ++x)
			{
				f32V3 const& rgb = rgbArray[y * width + x];
				SetPixel(rgb, Point<u32, 2>(x, y));
			}
		}

		Flush();
	}


	void MainWindow::SetPixel(f32V3 const& color, Point<u32, 2> position)
	{
		assert(position.X() < size_.X());
		assert(position.Y() < size_.Y());
		u8* lineStart = &hideWindows_->bitmapBits_[position.Y() * hideWindows_->lineStride_];
		reinterpret_cast<RGBStruct*>(lineStart)[position.X()] = RGBStruct(color);
	}

	void MainWindow::Flush()
	{
		::BitBlt(hideWindows_->hDC_, 0, 0, size_.X(), size_.Y(), hideWindows_->hDCMem_, 0, 0, SRCCOPY);
	}

	MainWindow::ColorSetter::~ColorSetter()
	{
		window_.Flush();
	}

	void MainWindow::ColorSetter::SetPixel(f32V3 const& color, Point<u32, 2> position)
	{
		window_.SetPixel(color, position);
	}

	void MainWindow::SetTitle(std::wstring const& title)
	{
		::SetWindowText(hideWindows_->hWnd_, title.c_str());
	}

	namespace
	{
		// copy~
		InputManager::InputSemantic InputSemanticFromWindowsVK(u32 winKey)
		{
			static std::array<InputManager::InputSemantic, static_cast<u32>(InputManager::InputSemantic::InputSemanticCount)> mapping = []
			{
				u32 const WindowsVKCount = 256;
				std::array<InputManager::InputSemantic, static_cast<u32>(InputManager::InputSemantic::InputSemanticCount)> mapping;
				mapping.fill(InputManager::InputSemantic::InputSemanticCount);

				mapping[0] = InputManager::InputSemantic::NullSemantic;

				mapping[VK_LBUTTON] = InputManager::InputSemantic::M_Button0;
				mapping[VK_RBUTTON] = InputManager::InputSemantic::M_Button1;
				mapping[VK_CANCEL] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_MBUTTON] = InputManager::InputSemantic::M_Button2;
				mapping[VK_XBUTTON1] = InputManager::InputSemantic::M_Button3;
				mapping[VK_XBUTTON2] = InputManager::InputSemantic::M_Button4;

				mapping[VK_BACK] = InputManager::InputSemantic::K_BackSpace;
				mapping[VK_TAB] = InputManager::InputSemantic::K_Tab;
				mapping[VK_CLEAR] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_RETURN] = InputManager::InputSemantic::K_Enter;

				mapping[VK_SHIFT] = InputManager::InputSemantic::Temp_Shift;
				mapping[VK_CONTROL] = InputManager::InputSemantic::Temp_Ctrl;
				mapping[VK_MENU] = InputManager::InputSemantic::Temp_Alt;
				mapping[VK_PAUSE] = InputManager::InputSemantic::K_Pause;
				mapping[VK_CAPITAL] = InputManager::InputSemantic::K_CapsLock;

				mapping[VK_KANA] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_HANGEUL] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_HANGUL] = InputManager::InputSemantic::InputSemanticCount;

				mapping[VK_JUNJA] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_FINAL] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_HANJA] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_KANJI] = InputManager::InputSemantic::InputSemanticCount;

				mapping[VK_ESCAPE] = InputManager::InputSemantic::K_Escape;
				mapping[VK_CONVERT] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_NONCONVERT] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_ACCEPT] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_MODECHANGE] = InputManager::InputSemantic::InputSemanticCount;

				mapping[VK_SPACE] = InputManager::InputSemantic::K_Space;
				mapping[VK_PRIOR] = InputManager::InputSemantic::K_PageUp;
				mapping[VK_NEXT] = InputManager::InputSemantic::K_PageDown;
				mapping[VK_END] = InputManager::InputSemantic::K_End;
				mapping[VK_HOME] = InputManager::InputSemantic::K_Home;
				mapping[VK_LEFT] = InputManager::InputSemantic::K_LeftArrow;
				mapping[VK_UP] = InputManager::InputSemantic::K_UpArrow;
				mapping[VK_RIGHT] = InputManager::InputSemantic::K_RightArrow;
				mapping[VK_DOWN] = InputManager::InputSemantic::K_DownArrow;
				mapping[VK_SELECT] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_PRINT] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_EXECUTE] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_SNAPSHOT] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_INSERT] = InputManager::InputSemantic::K_Insert;
				mapping[VK_DELETE] = InputManager::InputSemantic::K_Delete;
				mapping[VK_HELP] = InputManager::InputSemantic::InputSemanticCount;

				mapping['0'] = InputManager::InputSemantic::K_0;
				mapping['1'] = InputManager::InputSemantic::K_1;
				mapping['2'] = InputManager::InputSemantic::K_2;
				mapping['3'] = InputManager::InputSemantic::K_3;
				mapping['4'] = InputManager::InputSemantic::K_4;
				mapping['5'] = InputManager::InputSemantic::K_5;
				mapping['6'] = InputManager::InputSemantic::K_6;
				mapping['7'] = InputManager::InputSemantic::K_7;
				mapping['8'] = InputManager::InputSemantic::K_8;
				mapping['9'] = InputManager::InputSemantic::K_9;

				mapping['A'] = InputManager::InputSemantic::K_A;
				mapping['B'] = InputManager::InputSemantic::K_B;
				mapping['C'] = InputManager::InputSemantic::K_C;
				mapping['D'] = InputManager::InputSemantic::K_D;
				mapping['E'] = InputManager::InputSemantic::K_E;
				mapping['F'] = InputManager::InputSemantic::K_F;
				mapping['G'] = InputManager::InputSemantic::K_G;

				mapping['H'] = InputManager::InputSemantic::K_H;
				mapping['I'] = InputManager::InputSemantic::K_I;
				mapping['J'] = InputManager::InputSemantic::K_J;
				mapping['K'] = InputManager::InputSemantic::K_K;
				mapping['L'] = InputManager::InputSemantic::K_L;
				mapping['M'] = InputManager::InputSemantic::K_M;
				mapping['N'] = InputManager::InputSemantic::K_N;

				mapping['O'] = InputManager::InputSemantic::K_O;
				mapping['P'] = InputManager::InputSemantic::K_P;
				mapping['Q'] = InputManager::InputSemantic::K_Q;
				mapping['R'] = InputManager::InputSemantic::K_R;
				mapping['S'] = InputManager::InputSemantic::K_S;
				mapping['T'] = InputManager::InputSemantic::K_T;

				mapping['U'] = InputManager::InputSemantic::K_U;
				mapping['V'] = InputManager::InputSemantic::K_V;
				mapping['W'] = InputManager::InputSemantic::K_W;
				mapping['X'] = InputManager::InputSemantic::K_X;
				mapping['Y'] = InputManager::InputSemantic::K_Y;
				mapping['Z'] = InputManager::InputSemantic::K_Z;

				mapping[VK_LWIN] = InputManager::InputSemantic::K_LeftWin;
				mapping[VK_RWIN] = InputManager::InputSemantic::K_RightWin;
				mapping[VK_APPS] = InputManager::InputSemantic::K_Apps;

				mapping[VK_SLEEP] = InputManager::InputSemantic::K_Sleep;

				mapping[VK_NUMPAD0] = InputManager::InputSemantic::K_NumPad0;
				mapping[VK_NUMPAD1] = InputManager::InputSemantic::K_NumPad1;
				mapping[VK_NUMPAD2] = InputManager::InputSemantic::K_NumPad2;
				mapping[VK_NUMPAD3] = InputManager::InputSemantic::K_NumPad3;
				mapping[VK_NUMPAD4] = InputManager::InputSemantic::K_NumPad4;
				mapping[VK_NUMPAD5] = InputManager::InputSemantic::K_NumPad5;
				mapping[VK_NUMPAD6] = InputManager::InputSemantic::K_NumPad6;
				mapping[VK_NUMPAD7] = InputManager::InputSemantic::K_NumPad7;
				mapping[VK_NUMPAD8] = InputManager::InputSemantic::K_NumPad8;
				mapping[VK_NUMPAD9] = InputManager::InputSemantic::K_NumPad9;

				mapping[VK_MULTIPLY] = InputManager::InputSemantic::K_NumPadAsterisk;
				mapping[VK_ADD] = InputManager::InputSemantic::K_NumPadPlus;
				mapping[VK_SEPARATOR] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_SUBTRACT] = InputManager::InputSemantic::K_NumPadMinus;
				mapping[VK_DECIMAL] = InputManager::InputSemantic::K_NumPadPeriod;
				mapping[VK_DIVIDE] = InputManager::InputSemantic::K_NumPadSlash;

				mapping[VK_F1] = InputManager::InputSemantic::K_F1;
				mapping[VK_F2] = InputManager::InputSemantic::K_F2;
				mapping[VK_F3] = InputManager::InputSemantic::K_F3;
				mapping[VK_F4] = InputManager::InputSemantic::K_F4;
				mapping[VK_F5] = InputManager::InputSemantic::K_F5;
				mapping[VK_F6] = InputManager::InputSemantic::K_F6;
				mapping[VK_F7] = InputManager::InputSemantic::K_F7;
				mapping[VK_F8] = InputManager::InputSemantic::K_F8;
				mapping[VK_F9] = InputManager::InputSemantic::K_F9;
				mapping[VK_F10] = InputManager::InputSemantic::K_F10;
				mapping[VK_F11] = InputManager::InputSemantic::K_F11;
				mapping[VK_F12] = InputManager::InputSemantic::K_F12;
				mapping[VK_F13] = InputManager::InputSemantic::K_F13;
				mapping[VK_F14] = InputManager::InputSemantic::K_F14;
				mapping[VK_F15] = InputManager::InputSemantic::K_F15;
				mapping[VK_F16] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F17] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F18] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F19] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F20] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F21] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F22] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F23] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_F24] = InputManager::InputSemantic::InputSemanticCount;

				mapping[VK_NUMLOCK] = InputManager::InputSemantic::K_NumLock;
				mapping[VK_SCROLL] = InputManager::InputSemantic::K_ScrollLock;

				mapping[VK_OEM_NEC_EQUAL] = InputManager::InputSemantic::K_NumPadEquals;


				mapping[VK_LSHIFT] = InputManager::InputSemantic::K_LeftShift;
				mapping[VK_RSHIFT] = InputManager::InputSemantic::K_RightShift;
				mapping[VK_LCONTROL] = InputManager::InputSemantic::K_LeftCtrl;
				mapping[VK_RCONTROL] = InputManager::InputSemantic::K_RightCtrl;
				mapping[VK_LMENU] = InputManager::InputSemantic::K_LeftAlt;
				mapping[VK_RMENU] = InputManager::InputSemantic::K_RightAlt;

				mapping[VK_BROWSER_BACK] = InputManager::InputSemantic::K_WebBack;
				mapping[VK_BROWSER_FORWARD] = InputManager::InputSemantic::K_WebForward;
				mapping[VK_BROWSER_REFRESH] = InputManager::InputSemantic::K_WebRefresh;
				mapping[VK_BROWSER_STOP] = InputManager::InputSemantic::K_WebStop;
				mapping[VK_BROWSER_SEARCH] = InputManager::InputSemantic::K_WebSearch;
				mapping[VK_BROWSER_FAVORITES] = InputManager::InputSemantic::K_WebFavorites;
				mapping[VK_BROWSER_HOME] = InputManager::InputSemantic::K_WebHome;

				mapping[VK_VOLUME_MUTE] = InputManager::InputSemantic::K_Mute;
				mapping[VK_VOLUME_DOWN] = InputManager::InputSemantic::K_VolumeUp;
				mapping[VK_VOLUME_UP] = InputManager::InputSemantic::K_VolumeDown;
				mapping[VK_MEDIA_NEXT_TRACK] = InputManager::InputSemantic::K_NextTrack;
				mapping[VK_MEDIA_PREV_TRACK] = InputManager::InputSemantic::K_PrevTrack;
				mapping[VK_MEDIA_STOP] = InputManager::InputSemantic::K_MediaStop;
				mapping[VK_MEDIA_PLAY_PAUSE] = InputManager::InputSemantic::K_PlayPause;
				mapping[VK_LAUNCH_MAIL] = InputManager::InputSemantic::K_Mail;
				mapping[VK_LAUNCH_MEDIA_SELECT] = InputManager::InputSemantic::K_MediaSelect;
				mapping[VK_LAUNCH_APP1] = InputManager::InputSemantic::InputSemanticCount;
				mapping[VK_LAUNCH_APP2] = InputManager::InputSemantic::InputSemanticCount;

				mapping[VK_OEM_1] = InputManager::InputSemantic::K_Semicolon;
				mapping[VK_OEM_PLUS] = InputManager::InputSemantic::K_Equals;
				mapping[VK_OEM_COMMA] = InputManager::InputSemantic::K_Comma;
				mapping[VK_OEM_MINUS] = InputManager::InputSemantic::K_Minus;
				mapping[VK_OEM_PERIOD] = InputManager::InputSemantic::K_Period;
				mapping[VK_OEM_2] = InputManager::InputSemantic::K_Slash;
				mapping[VK_OEM_3] = InputManager::InputSemantic::K_Tilde;

				mapping[VK_OEM_4] = InputManager::InputSemantic::K_LeftBracket;
				mapping[VK_OEM_5] = InputManager::InputSemantic::K_BackSlash;
				mapping[VK_OEM_6] = InputManager::InputSemantic::K_RightBracket;
				mapping[VK_OEM_7] = InputManager::InputSemantic::K_Quote;
				mapping[VK_OEM_8] = InputManager::InputSemantic::InputSemanticCount;

				return mapping;
			} ();

			return mapping[winKey];
		}
	}

	void MainWindow::OnKeyDown(u32 winKey)
	{
		inputManager_->InjectKeyDown(InputSemanticFromWindowsVK(winKey));
	}

	void MainWindow::OnKeyUp(u32 winKey)
	{
		inputManager_->InjectKeyUp(InputSemanticFromWindowsVK(winKey));
	}

	void MainWindow::OnMouseDown(u32 winKey, u32 x, u32 y)
	{
		inputManager_->InjectMouseDown(InputSemanticFromWindowsVK(winKey), x, y);
	}

	void MainWindow::OnMouseUp(u32 winKey, u32 x, u32 y)
	{
		inputManager_->InjectMouseUp(InputSemanticFromWindowsVK(winKey), x, y);
	}

	void MainWindow::OnMouseWheel(u32 winKey, u32 x, u32 y, s32 wheelDelta)
	{
		inputManager_->InjectMouseWheel(InputManager::InputSemantic::M_Wheel, x, y, wheelDelta);
	}

	void MainWindow::OnMouseMove(u32 winKey, u32 x, u32 y)
	{
		inputManager_->InjectMouseMove(InputManager::InputSemantic::M_Move, x, size_.Y() - y);
	}


}
