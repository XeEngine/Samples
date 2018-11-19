#pragma once
#include <XeSDK.h>
#include <XeGame.h>

namespace HelloWorld {
	class HelloWorldView :
		public Xe::Core::IApplicationHandler,
		public Xe::Core::IFrameHandler,
		public Xe::Core::IKeyboardHandler,
		public Xe::Core::IPointerHandler
	{
		Xe::Core::IFrameView* m_pFrameView;
		Xe::Graphics::IContext* m_pContext;
		Xe::ObjPtr<Xe::Game::IDrawing2d> m_Drawing;

		bool m_IsVblankEnabled;
		bool m_IsFullscreen;
		Xe::ObjPtr<Xe::Graphics::ISurface> m_TexHelloWorld;

		void Draw();
		void SwapBuffer(bool isVblankEnabled);

		bool LoadImage(Xe::Graphics::Imaging::IImage** ppImage, Xe::Storage::IStorage* pStorage, ctstring fileName);

		bool LoadSurface(Xe::Graphics::ISurface** ppSurface, Xe::Storage::IStorage* pStorage, ctstring fileName);

		void LoadResources(Xe::Storage::IStorage* pStorage);
	public:
		HelloWorldView();
		~HelloWorldView();

		bool OnInitialize();
		void OnDestroy();
		void OnRun();
		void OnDraw();
		void OnSuspend();
		void OnResume();
		void OnDevice(const Xe::Core::DeviceArgs& args);

		bool OnAttach(Xe::Core::IFrameView* pFrameView);
		bool OnClosing(bool forced);
		void OnFocusGot();
		void OnFocusLost();
		void OnVisibilityChanged(bool visible);
		void OnSizeChanged(const Xe::Graphics::Size& size);
		void OnOrientationChanged(Xe::Core::Orientation orientation);
		void OnDpiChanged(float dpi);

		void OnCharacter(const Xe::IO::CharacterEvent& e);
		void OnKeyPressed(const Xe::IO::KeyboardEvent& e);
		void OnKeyReleased(const Xe::IO::KeyboardEvent& e);

		void OnPointerMoved(const Xe::IO::PointerEvent& e);
		void OnPointerPressed(const Xe::IO::PointerEvent& e);
		void OnPointerReleased(const Xe::IO::PointerEvent& e);
		void OnPointerEnter(const Xe::IO::PointerEvent& e);
		void OnPointerLeave(const Xe::IO::PointerEvent& e);
		void OnPointerWheel(const Xe::Math::Vector2<float>& wheel);
	};
}