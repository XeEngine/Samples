#include <XeSDK/IDelegate.h>
#include <XeGame/XeGameImGuiHandler.h>
#include <XeGame/IGameTilemapLayer.h>
#include <vector>

namespace PlatformGame
{
	class MapManager;
	class FrameMapInfo : public Xe::IDelegate<const Xe::Game::ImGuiFrameArgs&>
	{
		Xe::ObjPtr<MapManager> m_MapManager;
		std::vector<Xe::String> m_FilesList;
		bool m_IsComboOpen;

		void CtrlCameraSize();
		void CtrlBackgroundColor();
		void CtrlBufferSize(Xe::Game::ITilemapLayer* layer);
		void CtrlCameraPosition(Xe::Game::ITilemapLayer* layer);
		void FetchFilesList();
	public:
		FrameMapInfo(MapManager* mapManager);
		~FrameMapInfo();

		void operator()(const Xe::Game::ImGuiFrameArgs& args);
	};
}