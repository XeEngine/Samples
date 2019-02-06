#pragma once
#include <XeGame.h>
#include <XeGame/IGameTilemap2d.h>

namespace PlatformGame {
	class MapLoader :
		public Xe::Game::TilemapRequestTilesDelegate
	{
		Xe::String m_MapFileName;
		Xe::String m_MapDirectoryName;
		Xe::Game::Tiled::Map m_TiledMap;
		std::list<Xe::Game::Tiled::Tileset> m_Tilesets;

		void operator()(
			const Xe::Game::TilemapArgs<Xe::Game::TilemapRequestTilesArgs>& args);

		void LoadTileset(Xe::Game::Tiled::Tileset& tileset);
		Xe::String GetTilesetImagePath(const Xe::Game::Tiled::Tileset& tileset) const;
	public:

		const Xe::String& GetMap();
		bool LoadMap(const Xe::StringSpan& fileName);
		Xe::Math::Vector2i GetMapSize() const;
		Xe::Math::Vector2i GetTileSize() const;

		Xe::Graphics::Color GetBackgroundColor() const;
		int GetTilesPerRow() const;
		Xe::String GetTilesetImagePath() const;
	};
}