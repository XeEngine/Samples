#include "pch.h"
#include "PlatformGameView.h"
#include "FrameMapInfo.h"

using namespace Xe;
using namespace Xe::Game;

namespace Xe { namespace Core {
	// Entry point of our application.
	void Main() {
		Initialize();

		ObjPtr<PlatformGame::PlatformGameView> pCoreView = new PlatformGame::PlatformGameView;

		Xe::Core::FrameViewInitDesc viewInitDesc;
#ifdef DEVELOPMENT
		viewInitDesc.Title = "Platform game (dev)";
		viewInitDesc.Size.Set(1280, 720);
		viewInitDesc.IsResizable = true;
		viewInitDesc.IsFullscreen = false;
#else
		viewInitDesc.Title = "Platform game";
		viewInitDesc.Size.Set(1280, 720);
		viewInitDesc.IsResizable = true;
		viewInitDesc.IsFullscreen = true;
#endif

		if (Xe::Core::Run(pCoreView.Get(), viewInitDesc) == false)
		{
			LOGE("Xe::Core::Factory failed.\n");
		}
	}
} }

namespace PlatformGame
{
	PlatformGameView::PlatformGameView() :
		// Assign a default value to all members as pre-initialization.
		m_pFrameView(nullptr),
		m_ImGuiHandler(nullptr),
		m_pContext(nullptr),
		m_Input({ 0 }),
		m_IsVblankEnabled(true),
		m_IsFullscreen(false)
	{
	}

	PlatformGameView::~PlatformGameView()
	{
	}

	bool PlatformGameView::OnAttach(Xe::Core::IFrameView* pFrameView)
	{
		m_pFrameView = pFrameView;
		m_pFrameView->SetApplicationHandler(this);

		return true;
	}

	bool PlatformGameView::OnInitialize()
	{
		// Here belong the initialization of our view
		Xe::Graphics::ContextInitDesc contextInitDesc;
		contextInitDesc.FrameView = m_pFrameView;

		auto renderingDrivers = Xe::Drivers::GetDrivers<Xe::Drivers::IRenderingDriver>();
		//auto defaultRenderingDriver = *std::next(renderingDrivers.begin());
		auto defaultRenderingDriver = *renderingDrivers.begin();
		
		if (!defaultRenderingDriver->Factory(&m_pContext, contextInitDesc))
		{
			LOGE("Unable to initialize the default rendering driver %s.", defaultRenderingDriver->GetDriverName());
			return false;
		}

		// Set a default color as background of our app.
		m_pContext->SetClearColor(Graphics::Color::SlateGray);

		Factory(&m_TextureManager, m_pContext);

		m_MapManager = new MapManager(m_pContext, m_TextureManager);
		m_MapManager->LoadMap("sample_00.json");

		m_ImGuiHandler = new Xe::Game::ImGuiHandler(m_pContext);
		if (!m_ImGuiHandler->OnInitialize())
			return false;
		m_ImGuiHandler->OnSizeChanged(m_pFrameView->GetSize());

		// Set the handlers
		m_pFrameView->SetApplicationHandler(this);
		m_pFrameView->SetKeyboardHandler(this);
		m_pFrameView->SetPointerHandler(m_ImGuiHandler);

		m_Frames.push_back(new FrameMapInfo(m_MapManager));
		m_Frames.push_back(new Xe::Game::Tools::TilemapDebugger(m_pContext, m_TextureManager, m_MapManager->GetTilemap()));
		m_Frames.push_back(new Xe::Game::Tools::TextureDebugger(m_TextureManager));

		for (auto it = m_Frames.begin(); it != m_Frames.end(); ++it)
		{
			(*m_ImGuiHandler) += *it;
		}

		return true;
	}

	void PlatformGameView::OnDestroy()
	{
		for (auto it = m_Frames.begin(); it != m_Frames.end(); ++it)
		{
			auto& frame = *it;
			(*m_ImGuiHandler) -= frame;
			delete frame;
		}
		m_Frames.clear();

		m_ImGuiHandler->OnDestroy();
	}

	void PlatformGameView::OnRun()
	{
		// Slow down the app under 10fps
		static const double DELTATIME_TRESHOLD = 1.0 / 10.0;

		// This is the body of our application.
		Timer prev = Timer::Current();
		bool isRunning;

		do
		{
			try
			{
				while (isRunning = m_pFrameView->DispatchEvents(Core::DispatchType_One))
				{
					Timer cur = Timer::Current();
					double deltaTime = Timer(cur - prev).AsDouble();
					prev = cur;
					if (deltaTime > DELTATIME_TRESHOLD)
						deltaTime = DELTATIME_TRESHOLD;
				    
					HandleInput(deltaTime);
					m_MapManager->Update(deltaTime);

					Draw();
					SwapBuffer(m_IsVblankEnabled);
				}
			}
			catch (const std::exception& e)
			{
				LOGF("OnRun exception: %s", e.what());
			}
		} while (isRunning);
	}

	void PlatformGameView::OnDraw()
	{
		// Called when window needs to be redrawn
		Draw();
		SwapBuffer(m_IsVblankEnabled);
	}

	void PlatformGameView::OnSuspend()
	{
		m_ImGuiHandler->OnSuspend();
	}

	void PlatformGameView::OnResume()
	{
		// Check for the pad
		m_ImGuiHandler->OnResume();
	}

	void PlatformGameView::OnDevice(const Xe::Core::DeviceArgs& args)
	{
		// Check for the pad
		m_ImGuiHandler->OnDevice(args);
	}

	bool PlatformGameView::OnClosing(bool forced)
	{
		// Can ask for a confirmation message before to acutally close
		return m_ImGuiHandler->OnClosing(forced);
	}

	void PlatformGameView::OnFocusGot()
	{
		if (m_ImGuiHandler) m_ImGuiHandler->OnFocusGot();
	}

	void PlatformGameView::OnFocusLost()
	{
		m_ImGuiHandler->OnFocusLost();
	}

	void PlatformGameView::OnVisibilityChanged(bool visibility)
	{
		m_ImGuiHandler->OnVisibilityChanged(visibility);
	}

	void PlatformGameView::OnSizeChanged(const Graphics::Size& size)
	{
		if(m_pContext) m_pContext->SetInternalResolution(size);
		if (m_ImGuiHandler) m_ImGuiHandler->OnSizeChanged(size);
	}

	void PlatformGameView::OnOrientationChanged(Xe::Core::Orientation orientation)
	{
		m_ImGuiHandler->OnOrientationChanged(orientation);
	}

	void PlatformGameView::OnDpiChanged(float dpi)
	{
		m_ImGuiHandler->OnDpiChanged(dpi);
	}

	void PlatformGameView::OnCharacter(const IO::CharacterEvent& e)
	{
		m_ImGuiHandler->OnCharacter(e);
	}

	void PlatformGameView::OnKeyPressed(const IO::KeyboardEvent& e)
	{
		switch (e.VirtualCode) {
		case 0x25: case 0x41:
			m_Input.Left = 1;
			break;
		case 0x26: case 0x57:
			m_Input.Top = 1;
			break;
		case 0x27: case 0x44:
			m_Input.Right = 1;
			break;
		case 0x28: case 0x53:
			m_Input.Bottom = 1;
			break;
		case 0x73: // F4
			m_IsFullscreen = !m_IsFullscreen;
			m_pFrameView->SetFullScreen(m_IsFullscreen);
			break;
		default:
			m_ImGuiHandler->OnKeyPressed(e);
			break;
		}
	}
	void PlatformGameView::OnKeyReleased(const IO::KeyboardEvent& e)
	{
		switch (e.VirtualCode) {
		case 0x25: case 0x41:
			m_Input.Left = 0;
			break;
		case 0x26: case 0x57:
			m_Input.Top = 0;
			break;
		case 0x27: case 0x44:
			m_Input.Right = 0;
			break;
		case 0x28: case 0x53:
			m_Input.Bottom = 0;
			break;
		case 0x73: // F4
			m_IsFullscreen = !m_IsFullscreen;
			m_pFrameView->SetFullScreen(m_IsFullscreen);
			break;
		default:
			m_ImGuiHandler->OnKeyPressed(e);
			break;
		}

		m_ImGuiHandler->OnKeyReleased(e);

	}

	void PlatformGameView::HandleInput(double deltaTime)
	{
		const float Speed = 50.0f;

		Math::Vector2f movement;
		if (m_Input.Left) movement.x -= Speed * deltaTime;
		if (m_Input.Right) movement.x += Speed * deltaTime;
		if (m_Input.Top) movement.y -= Speed * deltaTime;
		if (m_Input.Bottom) movement.y += Speed * deltaTime;

		m_MapManager->SetCameraPosition(m_MapManager->GetCameraPosition() + movement);
	}

	void PlatformGameView::Draw()
	{
		m_pContext->Clear(Graphics::Clear_Color | Graphics::Clear_Depth | Graphics::Clear_Stencil);
		
		m_MapManager->Draw();
		m_ImGuiHandler->OnDraw();
	}

	void PlatformGameView::SwapBuffer(bool isVblankEnabled)
	{
		m_pContext->SwapBuffers(isVblankEnabled ?
			m_pContext->VBlank_Wait :
			m_pContext->VBlank_DoNotWait);
	}
}