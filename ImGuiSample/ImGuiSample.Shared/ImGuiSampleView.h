#pragma once
#include <XeSDK.h>
#include <XeGame.h>

namespace ImGuiSample {
	class ImGuiSampleView :
		public Xe::Core::IApplicationHandler,
		public Xe::Core::IFrameHandler
	{
		Xe::ObjPtr < Xe::Core::IFrameView> m_pFrameView;
		Xe::ObjPtr<Xe::Graphics::IContext> m_pContext;
		Xe::Game::ImGuiHandler* m_ImGuiHandler;
		std::list<Xe::IDelegate<const Xe::Game::ImGuiFrameArgs&>*> m_Frames;

		bool m_IsVblankEnabled;
		bool m_IsFullscreen;

		void Draw();
		void SwapBuffer(bool isVblankEnabled);
	public:
		ImGuiSampleView();
		~ImGuiSampleView();

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
	};
}