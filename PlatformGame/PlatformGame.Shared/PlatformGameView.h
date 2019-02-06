#pragma once
#include <XeSDK.h>
#include <XeGame.h>
#include "MapManager.h"

namespace PlatformGame {
	class PlatformGameView :
		public Xe::Core::IApplicationHandler,
		public Xe::Core::IFrameHandler,
		public Xe::Core::IKeyboardHandler
	{
		union Input
		{
			u32 Data;
			struct
			{
				u32 Left : 1;
				u32 Top : 1;
				u32 Right : 1;
				u32 Bottom : 1;
			};
		};

		Xe::ObjPtr<Xe::Core::IFrameView> m_pFrameView;
		Xe::ObjPtr<Xe::Graphics::IContext> m_pContext;
		Xe::Game::ImGuiHandler* m_ImGuiHandler;
		std::list<Xe::IDelegate<const Xe::Game::ImGuiFrameArgs&>*> m_Frames;

		Xe::ObjPtr<MapManager> m_MapManager;
		Xe::ObjPtr<Xe::Game::ITextureManager> m_TextureManager;

		Input m_Input;
		bool m_IsVblankEnabled;
		bool m_IsFullscreen;

		void HandleInput(double deltaTime);
		void Draw();
		void SwapBuffer(bool isVblankEnabled);
	public:
		PlatformGameView();
		~PlatformGameView();

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
	};
}