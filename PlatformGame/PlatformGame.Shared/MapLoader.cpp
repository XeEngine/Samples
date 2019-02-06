#include "pch.h"
#include "MapLoader.h"

using namespace Xe;
using namespace Xe::Error;
using namespace Xe::IO;
using namespace Xe::Storage;
using namespace Xe::Game;
using namespace PlatformGame;

const String& MapLoader::GetMap()
{
	return m_MapFileName;
}

bool MapLoader::LoadMap(const StringSpan& fileName)
{
	m_MapFileName = Path::Combine("data/map", fileName);
	m_MapDirectoryName = Path::GetDirectoryName(m_MapFileName);

	ObjPtr<IStorage> storage;
	GetStorage(&storage);

	ObjPtr<IFileStream> fileStream;
	if (storage->FileOpen(&fileStream, m_MapFileName, Access_Read, Mode_Open) != OK)
		return false;

	m_TiledMap = Xe::Game::Tiled::ParseJson(Tiled::Map(), fileStream);
	for (auto& tileset : m_TiledMap.Tilesets)
	{
		LoadTileset(tileset);
	}

	return true;
}

Xe::Math::Vector2i MapLoader::GetMapSize() const
{
	for (const auto& layer : m_TiledMap.Layers)
	{
		if (layer.Width > 0 && layer.Height > 0)
		{
			return { layer.Width, layer.Height };
		}
	}

	return Xe::Math::Vector2i();
}

Xe::Math::Vector2i MapLoader::GetTileSize() const
{
	return { m_TiledMap.TileWidth, m_TiledMap.TileHeight };
}

String MapLoader::GetTilesetImagePath(const Xe::Game::Tiled::Tileset& tileset) const
{
	return Path::Combine(m_MapDirectoryName, tileset.Image);
}

Xe::Graphics::Color MapLoader::GetBackgroundColor() const
{
	auto bgColor = StringSpan(m_TiledMap.BackgroundColor);
	if (bgColor.GetLength() > 0 && bgColor[0] == '#')
		bgColor = bgColor.Substring(1);

	int bgra;
	if (!bgColor.TryParse(bgra, 16))
		return Graphics::Color::Transparent;

	return Xe::Graphics::Color(bgra, 1.0f);
}

int MapLoader::GetTilesPerRow() const
{
	return (*m_TiledMap.Tilesets.begin()).Columns;
}

String MapLoader::GetTilesetImagePath() const
{
	if (m_TiledMap.Tilesets.size() <= 0)
		return String::Empty;

	const auto& tileset = *m_TiledMap.Tilesets.begin();
	return GetTilesetImagePath(tileset);
}

void MapLoader::LoadTileset(Xe::Game::Tiled::Tileset& tileset)
{
	ObjPtr<IStorage> storage;
	LOGFA(GetStorage(&storage));
	
	auto fullPath = Path::Combine(m_MapDirectoryName, tileset.Source);
	if (!storage->FileExists(fullPath))
		return;

	ObjPtr<IFileStream> fileStream;
	auto r = storage->FileOpen(&fileStream, fullPath, Access_Read, Mode_Open);
	if (r != Error::OK)
		return;

	Xe::Game::Tiled::ParseJson(tileset, fileStream);
}

void MapLoader::operator()(const TilemapArgs<TilemapRequestTilesArgs>& args)
{
	const auto& layer = *m_TiledMap.Layers.begin();

	auto& dst = args.Arguments->Destination;
	const auto& pos = args.Arguments->Position;

	int startX = Math::Max(0, -pos.x);
	int endX = Math::Min((int)dst.Size.x, layer.Width - pos.x);
	int endY = Math::Min((int)dst.Size.y, layer.Height - pos.y);
	for (int y = Math::Max(0, -pos.y); y < endY; ++y)
	{
		for (int x = startX; x < endX; ++x)
		{
			if (x < layer.Width && y < layer.Height)
			{
				auto tile = layer.Data[pos.x + x + (pos.y + y) * layer.Width];
				dst.Tile(x, y) = TileData{ tile.Data };
			}
			else
			{
				dst.Tile(x, y) = 0;
			}
		}
	}
}