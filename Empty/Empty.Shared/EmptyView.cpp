#include "pch.h"
#include "EmptyView.h"

using namespace Xe;

namespace Xe { namespace Core {
	// Entry point of our application.
	void Main() {
		Initialize();

		ObjPtr<Empty::EmptyView> pCoreView = new Empty::EmptyView;

		Xe::Core::FrameViewInitDesc viewInitDesc;
#ifdef DEVELOPMENT
		viewInitDesc.Title = "Empty (dev)";
		viewInitDesc.Size.Set(1280, 720);
		viewInitDesc.IsResizable = true;
		viewInitDesc.IsFullscreen = false;
#else
		viewInitDesc.Title = "Empty";
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

namespace Empty
{
	EmptyView::EmptyView() :
		// Assign a default value to all members as pre-initialization.
		m_pFrameView(nullptr),
		m_pContext(nullptr),
		m_IsVblankEnabled(true),
		m_IsFullscreen(false)
	{
	}

	EmptyView::~EmptyView()
	{
		// Use destructor to deallocate everything not destroyed by OnDestroy.
		if (m_pFrameView) m_pFrameView->Release();
	}

	bool EmptyView::OnAttach(Xe::Core::IFrameView* pFrameView)
	{
		m_pFrameView = pFrameView;
		m_pFrameView->AddRef();
		m_pFrameView->SetApplicationHandler(this);

		return true;
	}
	bool EmptyView::OnInitialize()
	{
		// Here belong the initialization of our view
		Xe::Graphics::ContextInitDesc contextInitDesc;
		contextInitDesc.FrameView = m_pFrameView;

		auto renderingDrivers = Xe::Drivers::GetDrivers(Xe::Drivers::DriverTypeFilter_Rendering);
		auto defaultRenderingDriver = (Xe::Drivers::Rendering::IRenderingDriver*)*renderingDrivers.begin();
		
		if (!defaultRenderingDriver->Factory(&m_pContext, contextInitDesc))
		{
			LOGE("Unable to initialize the default rendering driver %s.", defaultRenderingDriver->GetDriverName());
			return false;
		}

		// Set a default color as background of our app.
		m_pContext->SetClearColor(Graphics::Color::SlateGray);

		// Set the handlers
		m_pFrameView->SetApplicationHandler(this);
		m_pFrameView->SetKeyboardHandler(this);
		m_pFrameView->SetPointerHandler(this);

		return true;
	}
	void EmptyView::OnDestroy()
	{
		// This is called if OnInitialize was previously called, even if it
		// failed. Destroy here all allocated stuff.
		if (m_pContext) m_pContext->Release();
	}
	void EmptyView::OnRun()
	{
		static const double DELTATIME_TRESHOLD = 1.0 / 10.0;

		// This is the body of our application.
		Timer prev = Timer::Current();
		bool isRunning;

		do {
			try {
				while (isRunning = m_pFrameView->DispatchEvents(Core::DispatchType_One)) {
					Timer cur = Timer::Current();
					double deltaTime = Timer(cur - prev).AsDouble();
					prev = cur;
					if (deltaTime > DELTATIME_TRESHOLD)
						deltaTime = DELTATIME_TRESHOLD;

					// Process the logic

					Draw();
					SwapBuffer(m_IsVblankEnabled);
				}
			}
			catch (const std::exception& e) {
				LOGF("OnRun exception: %s", e.what());
			}
		} while (isRunning);
	}

	void EmptyView::OnDraw()
	{
		// Called when window needs to be redrawn
		Draw();
		SwapBuffer(m_IsVblankEnabled);
	}

	void EmptyView::OnSuspend()
	{
	}

	void EmptyView::OnResume()
	{
		// Check for the pad
	}

	void EmptyView::OnDevice(const Xe::Core::DeviceArgs& args)
	{
		// Check for the pad
	}

	bool EmptyView::OnClosing(bool forced)
	{
		// Can ask for a confirmation message before to acutally close
		return true;
	}

	void EmptyView::OnFocusGot()
	{
	}

	void EmptyView::OnFocusLost()
	{
	}

	void EmptyView::OnVisibilityChanged(bool visibility)
	{
	}

	void EmptyView::OnSizeChanged(const Graphics::Size& size)
	{
	}

	void EmptyView::OnOrientationChanged(Xe::Core::Orientation orientation)
	{
	}

	void EmptyView::OnDpiChanged(float dpi)
	{
	}

	void EmptyView::OnCharacter(const IO::CharacterEvent& e)
	{
	}

	void EmptyView::OnKeyPressed(const IO::KeyboardEvent& e)
	{
		switch (e.VirtualCode) {
		case 0x73: // F4
			m_IsFullscreen = !m_IsFullscreen;
			m_pFrameView->SetFullScreen(m_IsFullscreen);
			break;
		default:
			break;
		}

	}
	void EmptyView::OnKeyReleased(const IO::KeyboardEvent& e)
	{
	}

	void EmptyView::OnPointerMoved(const IO::PointerEvent& e)
	{
	}

	void EmptyView::OnPointerPressed(const IO::PointerEvent& e)
	{
	}

	void EmptyView::OnPointerReleased(const IO::PointerEvent& e)
	{
	}

	void EmptyView::OnPointerEnter(const IO::PointerEvent& e)
	{
	}

	void EmptyView::OnPointerLeave(const IO::PointerEvent& e)
	{
	}

	void EmptyView::OnPointerWheel(const Math::Vector2<float>& wheel)
	{
	}

	void EmptyView::Draw() {
		m_pContext->Clear(Graphics::Clear_Color | Graphics::Clear_Depth | Graphics::Clear_Stencil);
	}

	void EmptyView::SwapBuffer(bool isVblankEnabled)
	{
		m_pContext->SwapBuffers(isVblankEnabled ?
			m_pContext->VBlank_Wait :
			m_pContext->VBlank_DoNotWait);
	}
}