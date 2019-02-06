#pragma once
#include "MapLoader.h"
#include "MapDrawer.h"
#include <XeGame/IGameTilemap2d.h>

namespace PlatformGame {
	class MapManager :
		public Xe::IObject
	{
		MapLoader m_Loader;
		MapDrawer m_Drawer;

		Xe::ObjPtr<Xe::Graphics::IContext> m_Context;
		Xe::ObjPtr<Xe::Game::ITextureManager> m_TextureManager;
		Xe::ObjPtr<Xe::Game::ITilemap2d> m_Tilemap;

		Xe::String m_TilesetFileName;
		Xe::Game::TexId m_TilesetTextureId;

		void Draw(const Xe::Game::TilemapDrawList& drawList);
		Xe::Game::TexId LoadTileset();
		Xe::Game::TexId LoadTileset(const Xe::String& fileName);
		void UnloadTileset();

		void AdjustBufferSize();
		static size_t GetRequiredBufferSize(int requiredLength, int multiple);

	public:
		MapManager(
			Xe::Graphics::IContext* pContext,
			Xe::Game::ITextureManager* pTextureManager);
		~MapManager();

		Xe::ObjPtr<Xe::Game::ITilemap2d> GetTilemap();
		size_t GetLayerCount() const;
		Xe::ObjPtr<Xe::Game::ITilemapLayer> GetLayer(size_t index);

		const Xe::Math::Vector2i& GetCameraSize() const;
		void SetCameraSize(const Xe::Math::Vector2i& cameraSize);

		const Xe::Math::Vector2f& GetCameraPosition() const;
		void SetCameraPosition(const Xe::Math::Vector2f& cameraPosition);

		const Xe::String& GetMap();
		Xe::Math::Vector2i GetMapSize();
		bool LoadMap(const Xe::StringSpan& fileName);

		const Xe::Graphics::Color& GetBackgroundColor() const;
		void SetBackgroundColor(const Xe::Graphics::Color& color);

		const Xe::String& GetTilesetFileName() const;
		Xe::Game::TexId GetTilesetTextureId() const;
		Xe::Game::ITextureManager* GetTextureManager() const;

		Xe::Math::Vector2f CheckBoundaries(const Xe::Math::Vector2f& position);

		void Update(double deltaTime);
		void Draw();
	};
}