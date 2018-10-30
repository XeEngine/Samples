#include <XeSDK/IDelegate.h>
#include <XeGame/XeGameImGuiHandler.h>

namespace ImGuiSample
{
	class FrameEngineInfo : public Xe::IDelegate<const Xe::Game::ImGuiFrameArgs&>
	{
		Xe::Core::EngineInfo m_EngineInfo;
		Xe::Core::HostInfo m_HostInfo;
	public:
		FrameEngineInfo();

		void operator()(const Xe::Game::ImGuiFrameArgs& args);
	};
}