#include "pch.h"
#include "MapManager.h"
#include <XeGame/XeGameTilemap2d.h>

using namespace Xe;
using namespace Xe::Graphics;
using namespace Xe::Graphics::Imaging;
using namespace Xe::Storage;
using namespace Xe::Game;
using namespace PlatformGame;

MapManager::MapManager(
	IContext* pContext,
	ITextureManager* pTextureManager) :
	m_Context(pContext),
	m_TextureManager(pTextureManager)
{
	Xe::Game::Factory(&m_Tilemap, nullptr);
	m_Tilemap->SetRequestTilesCallback(&m_Loader);

	m_Tilemap->SetCameraSize({ 256, 144 });
	m_Tilemap->SetLayersCount(2);

	auto layer = m_Tilemap->GetLayer(0);
	layer->SetName("Layer 0");
	layer->SetBufferSize({ 48, 24 });
	layer->SetVisible(true);
}

MapManager::~MapManager()
{
	UnloadTileset();
}

Xe::ObjPtr<Xe::Game::ITilemap2d> MapManager::GetTilemap()
{
	return m_Tilemap;
}

size_t MapManager::GetLayerCount() const
{
	return m_Tilemap->GetLayerCount();
}

Xe::ObjPtr<Xe::Game::ITilemapLayer> MapManager::GetLayer(size_t index)
{
	return m_Tilemap->GetLayer(index);
}

const Xe::Math::Vector2i& MapManager::GetCameraSize() const
{
	return m_Tilemap->GetCameraSize();
}

void MapManager::SetCameraSize(const Xe::Math::Vector2i& cameraSize)
{
	m_Tilemap->SetCameraSize(cameraSize);
	AdjustBufferSize();
}

const Xe::Math::Vector2f& MapManager::GetCameraPosition() const
{
	return m_Tilemap->GetCameraPosition();
}

void MapManager::SetCameraPosition(const Xe::Math::Vector2f& cameraPosition)
{
	m_Tilemap->SetCameraPosition(CheckBoundaries(cameraPosition));
}

const Xe::String& MapManager::GetMap()
{
	return m_Loader.GetMap();
}

Xe::Math::Vector2i MapManager::GetMapSize()
{
	return m_Loader.GetMapSize();
}

bool MapManager::LoadMap(const Xe::StringSpan& fileName)
{
	UnloadTileset();

	if (!m_Loader.LoadMap(fileName))
		return false;

	SetBackgroundColor(m_Loader.GetBackgroundColor());
	m_Tilemap->SetTileSize(m_Loader.GetTileSize());
	AdjustBufferSize();

	m_Tilemap->GetLayer(0)->SetTexture(LoadTileset(), m_Loader.GetTilesPerRow());

	TileFrame frames[] = { { 120, 80.0f }, { 121, 80.0f }, { 122, 40.0f }, { 123, 80.0f } };
	m_Tilemap->AddTileSequence(TileData{ 120 + 1 }, frames);

	return true;
}

const Xe::Graphics::Color& MapManager::GetBackgroundColor() const
{
	return m_Tilemap->GetBackgroundColor();
}

void MapManager::SetBackgroundColor(const Xe::Graphics::Color& color)
{
	m_Tilemap->SetBackgroundColor(color);
}

const Xe::String& MapManager::GetTilesetFileName() const
{
	return m_TilesetFileName;
}

Xe::Game::TexId MapManager::GetTilesetTextureId() const
{
	return m_TilesetTextureId;
}

Xe::Game::ITextureManager* MapManager::GetTextureManager() const
{
	return m_TextureManager.Get();
}

Xe::Math::Vector2f MapManager::CheckBoundaries(const Xe::Math::Vector2f& position)
{
	Xe::Math::Vector2f newPos;
	auto mapSize = GetMapSize() * m_Tilemap->GetTileSize();
	const auto& camSize = GetCameraSize();

	newPos.x = Math::Max(Math::Min(position.x, (float)(mapSize.x - camSize.x)), 0.0f);
	newPos.y = Math::Max(Math::Min(position.y, (float)(mapSize.y - camSize.y)), 0.0f);
	return newPos;
}

void MapManager::Update(double deltaTime)
{
	m_Tilemap->Update(deltaTime);
}

void MapManager::Draw()
{
	m_Tilemap->Flush();

	Xe::Game::TilemapDrawArgs args;
	m_Tilemap->Draw(args, Xe::Game::TilemapDraw_All);

	for (const auto& draw : args.Draws)
	{
		Draw(draw);
	}
}

TexId MapManager::LoadTileset()
{
	m_TilesetFileName = m_Loader.GetTilesetImagePath();
	if (m_TilesetFileName.IsEmpty())
		return TexInvalid;

	return m_TilesetTextureId = LoadTileset(m_TilesetFileName);
}

TexId MapManager::LoadTileset(const Xe::String& fileName)
{
	bool r;
	ObjPtr<IStorage> storage;
	GetStorage(&storage);

	ObjPtr<IImage> image;
	LOGFA(r = OpenImage(&image, storage, fileName));
	return r ? m_TextureManager->Create(image, fileName) : TexInvalid;
}

void MapManager::UnloadTileset()
{
	m_TilesetFileName = String::Empty;
	m_TilesetTextureId = TexInvalid;
}

void MapManager::Draw(const TilemapDrawList& drawList)
{
	const auto& cameraSize = GetCameraSize();
	Xe::Math::Rectanglef rect =
		Xe::Math::Rectanglef::FromSize({ 0, 0 }, { (float)cameraSize.x, (float)cameraSize.y });
	Xe::Game::Draw(drawList, m_Context, m_TextureManager, rect);
}

void MapManager::AdjustBufferSize()
{
	const int Multiplier = 8;
	const auto& cameraSize = GetCameraSize();
	const auto& tileSize = m_Tilemap->GetTileSize();
	int tileHCount = cameraSize.x / tileSize.x + 1;
	int tileVCount = cameraSize.y / tileSize.y + 1;

	m_Tilemap->GetLayer(0)->SetBufferSize(
		{
			GetRequiredBufferSize(tileHCount, Multiplier),
			GetRequiredBufferSize(tileVCount, Multiplier)
		});
}

size_t MapManager::GetRequiredBufferSize(int requiredLength, int multiple)
{
	size_t bufferLength = multiple;
	while (requiredLength >= bufferLength)
		bufferLength += multiple;

	return bufferLength;
}
