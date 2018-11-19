#include "pch.h"
#include "ImGuiSampleView.h"
#include "FrameHelloWorld.h"
#include "FrameEngineInfo.h"

using namespace Xe;

namespace Xe { namespace Core {
	// Entry point of our application.
	void Main() {
		Initialize();

		ObjPtr<ImGuiSample::ImGuiSampleView> pCoreView = new ImGuiSample::ImGuiSampleView;

		Xe::Core::FrameViewInitDesc viewInitDesc;
#ifdef DEVELOPMENT
		viewInitDesc.Title = "ImGui Sample (dev)";
		viewInitDesc.Size.Set(1280, 720);
		viewInitDesc.IsResizable = true;
		viewInitDesc.IsFullscreen = false;
#else
		viewInitDesc.Title = "ImGui Sample";
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

namespace ImGuiSample
{
	ImGuiSampleView::ImGuiSampleView() :
		// Assign a default value to all members as pre-initialization.
		m_pFrameView(nullptr),
		m_ImGuiHandler(nullptr),
		m_IsVblankEnabled(true),
		m_IsFullscreen(false)
	{
	}

	ImGuiSampleView::~ImGuiSampleView()
	{
		if (m_ImGuiHandler) m_ImGuiHandler->Release();
	}

	bool ImGuiSampleView::OnAttach(Xe::Core::IFrameView* pFrameView)
	{
		m_pFrameView = pFrameView;
		m_pFrameView->SetApplicationHandler(this);

		return true;
	}
	bool ImGuiSampleView::OnInitialize()
	{
		// Here belong the initialization of our view
		Xe::Graphics::ContextInitDesc contextInitDesc;
		contextInitDesc.FrameView = m_pFrameView;

		auto renderingDrivers = Xe::Drivers::GetDrivers<Xe::Drivers::IRenderingDriver>();
		auto defaultRenderingDriver = *renderingDrivers.begin();
		
		if (!defaultRenderingDriver->Factory(&m_pContext, contextInitDesc))
		{
			LOGE("Unable to initialize the default rendering driver %s.", defaultRenderingDriver->GetDriverName());
			return false;
		}

		m_ImGuiHandler = new Xe::Game::ImGuiHandler(m_pContext);
		if (!m_ImGuiHandler->OnInitialize())
			return false;
		m_ImGuiHandler->OnSizeChanged(m_pFrameView->GetSize());

		m_Frames.push_back(new FrameHelloWorld);
		m_Frames.push_back(new FrameEngineInfo);
		for (auto it = m_Frames.begin(); it != m_Frames.end(); ++it)
		{
			(*m_ImGuiHandler) += *it;
		}

		// Set a default color as background of our app.
		m_pContext->SetClearColor(Graphics::Color::SlateGray);

		// Set the handlers
		m_pFrameView->SetApplicationHandler(this);
		m_pFrameView->SetKeyboardHandler(m_ImGuiHandler);
		m_pFrameView->SetPointerHandler(m_ImGuiHandler);

		return m_ImGuiHandler;
	}
	void ImGuiSampleView::OnDestroy()
	{
		// This is called if OnInitialize was previously called, even if it
		// failed. Destroy here all allocated stuff.
		for (auto it = m_Frames.begin(); it != m_Frames.end(); ++it)
		{
			(*m_ImGuiHandler) -= *it;
			delete *it;
		}
		m_Frames.clear();

		m_ImGuiHandler->OnDestroy();
		m_pContext = nullptr;
	}
	void ImGuiSampleView::OnRun()
	{
		static const double DELTATIME_TRESHOLD = 1.0 / 10.0;

		// This is the body of our application.
		Timer prev = Timer::Current();
		bool isRunning;

		do {
			try {

				while (isRunning = m_pFrameView->DispatchEvents(Core::DispatchType_All)) {
					Draw();
					SwapBuffer(m_IsVblankEnabled);
				}
			}
			catch (const std::exception& e) {
				LOGF("OnRun exception: %s", e.what());
			}
		} while (isRunning);
	}

	void ImGuiSampleView::OnDraw()
	{
		// Called when window needs to be redrawn
		Draw();
		SwapBuffer(m_IsVblankEnabled);
	}

	void ImGuiSampleView::OnSuspend()
	{
		m_ImGuiHandler->OnSuspend();
	}

	void ImGuiSampleView::OnResume()
	{
		// Check for the pad
		m_ImGuiHandler->OnResume();
	}

	void ImGuiSampleView::OnDevice(const Xe::Core::DeviceArgs& args)
	{
		// Check for the pad
		m_ImGuiHandler->OnDevice(args);
	}

	bool ImGuiSampleView::OnClosing(bool forced)
	{
		// Can ask for a confirmation message before to acutally close
		return m_ImGuiHandler->OnClosing(forced);
	}

	void ImGuiSampleView::OnFocusGot()
	{
		if (m_ImGuiHandler) m_ImGuiHandler->OnFocusGot();
	}

	void ImGuiSampleView::OnFocusLost()
	{
		m_ImGuiHandler->OnFocusLost();
	}

	void ImGuiSampleView::OnVisibilityChanged(bool visibility)
	{
		m_ImGuiHandler->OnVisibilityChanged(visibility);
	}

	void ImGuiSampleView::OnSizeChanged(const Graphics::Size& size)
	{
		if (m_ImGuiHandler) m_ImGuiHandler->OnSizeChanged(size);
	}

	void ImGuiSampleView::OnOrientationChanged(Xe::Core::Orientation orientation)
	{
		m_ImGuiHandler->OnOrientationChanged(orientation);
	}

	void ImGuiSampleView::OnDpiChanged(float dpi)
	{
		m_ImGuiHandler->OnDpiChanged(dpi);
	}


	void ImGuiSampleView::Draw() {
		m_pContext->Clear(Graphics::Clear_Color | Graphics::Clear_Depth | Graphics::Clear_Stencil);
		m_ImGuiHandler->OnDraw();
	}

	void ImGuiSampleView::SwapBuffer(bool isVblankEnabled)
	{
		m_pContext->SwapBuffers(isVblankEnabled ?
			m_pContext->VBlank_Wait :
			m_pContext->VBlank_DoNotWait);
	}
}