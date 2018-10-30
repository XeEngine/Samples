#include "pch.h"
#include "HelloWorldView.h"

using namespace Xe;
using namespace Xe::IO;
using namespace Xe::Graphics;
using namespace Xe::Graphics::Imaging;
using namespace Xe::Math;
using namespace Xe::Storage;

namespace Xe { namespace Core {
	// Entry point of our application.
	void Main() {
		Initialize();

		ObjPtr<HelloWorld::HelloWorldView> pCoreView = new HelloWorld::HelloWorldView;

		Xe::Core::FrameViewInitDesc viewInitDesc;
#ifdef DEVELOPMENT
		viewInitDesc.Title = "Hello World (dev)";
		viewInitDesc.Size.Set(1280, 720);
		viewInitDesc.IsResizable = true;
		viewInitDesc.IsFullscreen = false;
#else
		viewInitDesc.Title = "Hello World";
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

namespace HelloWorld
{
	HelloWorldView::HelloWorldView() :
		// Assign a default value to all members as pre-initialization.
		m_pFrameView(nullptr),
		m_pContext(nullptr),
		m_IsVblankEnabled(true),
		m_IsFullscreen(false)
	{
	}

	HelloWorldView::~HelloWorldView()
	{
		// Use destructor to deallocate everything not destroyed by OnDestroy.
		if (m_pFrameView) m_pFrameView->Release();
	}

	bool HelloWorldView::OnAttach(Xe::Core::IFrameView* pFrameView)
	{
		m_pFrameView = pFrameView;
		m_pFrameView->AddRef();
		m_pFrameView->SetApplicationHandler(this);

		return true;
	}
	bool HelloWorldView::OnInitialize()
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

		ObjPtr<IStorage> storage;
		if (GetStorage(&storage))
		{
			LoadResources(storage);
		}

		return true;
	}
	void HelloWorldView::OnDestroy()
	{
		// This is called if OnInitialize was previously called, even if it
		// failed. Destroy here all allocated stuff.
		if (m_pContext) m_pContext->Release();
	}
	void HelloWorldView::OnRun()
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

	void HelloWorldView::OnDraw()
	{
		// Called when window needs to be redrawn
		Draw();
		SwapBuffer(m_IsVblankEnabled);
	}

	void HelloWorldView::OnSuspend()
	{
	}

	void HelloWorldView::OnResume()
	{
		// Check for the pad
	}

	void HelloWorldView::OnDevice(const Xe::Core::DeviceArgs& args)
	{
		// Check for the pad
	}

	bool HelloWorldView::OnClosing(bool forced)
	{
		// Can ask for a confirmation message before to acutally close
		return true;
	}

	void HelloWorldView::OnFocusGot()
	{
	}

	void HelloWorldView::OnFocusLost()
	{
	}

	void HelloWorldView::OnVisibilityChanged(bool visibility)
	{
	}

	void HelloWorldView::OnSizeChanged(const Graphics::Size& size)
	{
	}

	void HelloWorldView::OnOrientationChanged(Xe::Core::Orientation orientation)
	{
	}

	void HelloWorldView::OnDpiChanged(float dpi)
	{
	}

	void HelloWorldView::OnCharacter(const IO::CharacterEvent& e)
	{
	}

	void HelloWorldView::OnKeyPressed(const IO::KeyboardEvent& e)
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
	void HelloWorldView::OnKeyReleased(const IO::KeyboardEvent& e)
	{
	}

	void HelloWorldView::OnPointerMoved(const IO::PointerEvent& e)
	{
	}

	void HelloWorldView::OnPointerPressed(const IO::PointerEvent& e)
	{
	}

	void HelloWorldView::OnPointerReleased(const IO::PointerEvent& e)
	{
	}

	void HelloWorldView::OnPointerEnter(const IO::PointerEvent& e)
	{
	}

	void HelloWorldView::OnPointerLeave(const IO::PointerEvent& e)
	{
	}

	void HelloWorldView::OnPointerWheel(const Math::Vector2<float>& wheel)
	{
	}

	bool HelloWorldView::LoadImage(Xe::Graphics::Imaging::IImage** ppImage, Xe::Storage::IStorage* pStorage, ctstring fileName)
	{
		ObjPtr<IFileStream> fileStream;
		if (pStorage->FileOpen(&fileStream, fileName, Access_Read, Mode_Open) == Error::OK)
		{
			if (PngOpen(ppImage, fileStream))
			{
				return true;
			}

			LOGW("%s is not a PNG file", fileName);
			return false;
		}

		LOGW("%s not found", fileName);
		return false;
	}

	bool HelloWorldView::LoadSurface(Xe::Graphics::ISurface** ppSurface, Xe::Storage::IStorage* pStorage, ctstring fileName)
	{
		ObjPtr<IImage> image;
		if (LoadImage(&image, pStorage, fileName))
		{
			return CreateSurface(m_pContext, ppSurface, Xe::Graphics::SurfaceType_Texture, image);
		}

		LOGE("Unable to load %s", fileName);
		return false;
	}

	void HelloWorldView::LoadResources(Xe::Storage::IStorage* pStorage)
	{
		LoadSurface(&m_TexHelloWorld, pStorage, "XeEngine-Logo.png");
	}

	void HelloWorldView::Draw() {
		m_pContext->Clear(Graphics::Clear_Color | Graphics::Clear_Depth | Graphics::Clear_Stencil);

		ObjPtr<IDrawing2d> drawing;
		m_pContext->GetDrawing(&drawing);
		m_pContext->SelectSurface(m_TexHelloWorld, 0);

		static const Vector2f POS[2]
		{
			Vector2f(-1.0f, +1.0f),
			Vector2f(+1.0f, -1.0f)
		};

		static const Vector2f UV[4]
		{
			Vector2f(0.0f, 0.0f),
			Vector2f(1.0f, 0.0f),
			Vector2f(0.0f, 1.0f),
			Vector2f(1.0f, 1.0f)
		};

		drawing->DrawSurface(POS, UV, Color::White, IDrawing2d::MODE_TEXTURED);
		drawing->Flush();
	}

	void HelloWorldView::SwapBuffer(bool isVblankEnabled)
	{
		m_pContext->SwapBuffers(isVblankEnabled ?
			m_pContext->VBlank_Wait :
			m_pContext->VBlank_DoNotWait);
	}
}