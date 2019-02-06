#include "pch.h"
#include "FrameMapInfo.h"
#include <imgui/imgui.h>

#include "MapManager.h"

using namespace Xe;
using namespace Xe::Error;
using namespace Xe::IO;
using namespace Xe::Storage;
using namespace Xe::Game;

namespace PlatformGame
{
	FrameMapInfo::FrameMapInfo(MapManager* mapManager) :
		m_MapManager(mapManager)
	{ }

	FrameMapInfo::~FrameMapInfo()
	{

	}

	void FrameMapInfo::operator()(const Xe::Game::ImGuiFrameArgs& args)
	{
		if (ImGui::Begin("MapManager"))
		{
			if (bool isComboOpen = ImGui::BeginCombo("Map", m_MapManager->GetMap()))
			{
				if (!m_IsComboOpen)
				{
					FetchFilesList();
					m_IsComboOpen = true;
				}

				for (const auto& fileName : m_FilesList)
				{
					bool isSelected = false;
					if (ImGui::Selectable(fileName, &isSelected))
					{
						m_MapManager->LoadMap(fileName);
					}
				}

				ImGui::EndCombo();
			}
			else
			{
				m_IsComboOpen = false;
			}

			const auto& tilesetFileName = m_MapManager->GetTilesetFileName();
			ImGui::InputText("Tileset", (char*)tilesetFileName.GetData(), tilesetFileName.GetLength(), ImGuiInputTextFlags_ReadOnly);

			const auto& tilesetTextureId = m_MapManager->GetTilesetTextureId();
			if (tilesetTextureId != TexInvalid)
			{
				const auto& textureManager = m_MapManager->GetTextureManager();
				const auto& surface = textureManager->GetSurface(tilesetTextureId);

				ImGui::Text("Tileset loaded with ID %i!", tilesetTextureId);
			}
			else
			{
				ImGui::Text("Tileset texture not loaded or invalid");
			}

			ImGui::Separator();
			
			CtrlCameraSize();
			CtrlBackgroundColor();
			
			ImGui::Separator();
			
			if (m_MapManager->GetLayerCount() > 0)
			{
				auto layer = m_MapManager->GetLayer(0);
				CtrlBufferSize(layer);
				CtrlCameraPosition(layer);
			}

			ImGui::End();
		}
	}

	void FrameMapInfo::CtrlCameraSize()
	{
		const auto& cameraSize = m_MapManager->GetCameraSize();
		int v[2]{ cameraSize.x, cameraSize.y };
		if (ImGui::InputInt2("Camera size", v))
		{
			m_MapManager->SetCameraSize({ v[0], v[1] });
		}
	}

	void FrameMapInfo::CtrlBackgroundColor()
	{
		auto bgColor = m_MapManager->GetBackgroundColor();
		float v[4]{ bgColor.b, bgColor.g, bgColor.r, bgColor.a };
		if (ImGui::ColorEdit4("Background", v))
		{
			m_MapManager->SetBackgroundColor({ v[2], v[1], v[0], v[3] });
		}
	}

	void FrameMapInfo::CtrlCameraPosition(Xe::Game::ITilemapLayer* layer)
	{
	}

	void FrameMapInfo::CtrlBufferSize(Xe::Game::ITilemapLayer* layer)
	{
		const auto& bufferSize = layer->GetBufferSize();
		int v[2]{ (int)bufferSize.x, (int)bufferSize.y };
		if (ImGui::InputInt2("Buffer size", v, ImGuiInputTextFlags_ReadOnly))
		{
			layer->SetBufferSize({ (size_t)v[0], (size_t)v[1] });
		}
	}

	void FrameMapInfo::FetchFilesList()
	{
		ObjPtr<IStorage> storage;
		GetStorage(&storage);

		IDirectory* pDirectory;
		if (storage->DirectoryOpen(&pDirectory, "data/map") != OK)
			return;

		m_FilesList.clear();
		while (pDirectory->Next())
		{
			auto name = pDirectory->GetName();
			if (Path::GetExtension(name) != ".json")
				continue;

			m_FilesList.push_back(String(name));
		}
	}
}