#include <XeSDK/IDelegate.h>
#include <XeGame/XeGameImGuiHandler.h>

namespace ImGuiSample
{
	class FrameHelloWorld : public Xe::IDelegate<const Xe::Game::ImGuiFrameArgs&>
	{
		Xe::Graphics::Color m_ForeHsvColor;
	public:
		void operator()(const Xe::Game::ImGuiFrameArgs& args);
	};
}