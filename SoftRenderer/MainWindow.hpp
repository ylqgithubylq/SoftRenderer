#pragma once
#include "Common.hpp"
#include "Setting.hpp"

namespace X
{
	class MainWindow
		: Noncopyable
	{
	public:
		MainWindow(Setting const& setting);
		~MainWindow();

		void SetInputManager(InputManager* inputManager)
		{
			inputManager_ = inputManager;
		}
		/*
		*	@return: false to quit the application.
		*/
		bool HandleMessage();

		Size<u32, 2> const& GetClientRegionSize() const
		{
			return size_;
		}

		void SetRendering(bool rendering)
		{
			rendering_ = rendering;
		}

		void DrawColorRectangle(f32V3 const* rgbArray, u32 width, u32 height);

	private:
		void SetPixel(f32V3 const& color, Point<u32, 2> position);
		void Flush();

	public:
		struct ColorSetter
		{
			friend class MainWindow;
		private:
			ColorSetter(MainWindow& window)
				: window_(window)
			{
			}
		public:
			~ColorSetter();

			void SetPixel(f32V3 const& color, Point<u32, 2> position);

		private:
			MainWindow& window_;
		};

		ColorSetter GetColorSetter()
		{
			return ColorSetter(*this);
		}

		void SetTitle(std::wstring const& title);

	private:
		void OnKeyDown(u32 winKey);
		void OnKeyUp(u32 winKey);
		void OnMouseDown(u32 winKey, u32 x, u32 y);
		void OnMouseUp(u32 winKey, u32 x, u32 y);
		void OnMouseWheel(u32 winKey, u32 x, u32 y, s32 wheelDelta);
		void OnMouseMove(u32 winKey, u32 x, u32 y);


	private:

		// hide windows.h to the cpp file
		struct HideWindows_;
		std::unique_ptr<HideWindows_> hideWindows_;

		InputManager* inputManager_;

		bool active_;
		bool running_;
		bool rendering_;

		Size<u32, 2> size_;

		s32 clientRegionLeft_;
		s32 clientRegionTop_;

		s32 windowLeft_;
		s32 windowTop_;

		std::wstring name_;

	};

}

