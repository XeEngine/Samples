#include "pch.h"
#include "FrameEngineInfo.h"
#include <imgui/imgui.h>

namespace ImGuiSample
{
	FrameEngineInfo::FrameEngineInfo() :
		m_EngineInfo(Xe::Core::GetEngineInfo()),
		m_HostInfo(Xe::Core::GetHostInfo())
	{ }

	void FrameEngineInfo::operator()(const Xe::Game::ImGuiFrameArgs& args)
	{
		if (ImGui::Begin("Info"))
		{
			ImGui::Text("%s %s", m_EngineInfo.Name, m_EngineInfo.StrVersion);
			ImGui::Text("Developed by %s", m_EngineInfo.Developer);
			ImGui::Text("Build date: %s", m_EngineInfo.BuildDate);
			ImGui::Text("Compiler: %s %i ", m_EngineInfo.Compiler, m_EngineInfo.CompilerVersion);
			ImGui::Separator();
			ImGui::Text("%s %i.%i", m_HostInfo.FriendlyName, m_HostInfo.OsVersion.Major, m_HostInfo.OsVersion.Minor);
			ImGui::Text("SDK ver. %X", m_HostInfo.SdkVersion);
			ImGui::Text("Processor count: %X", m_HostInfo.ProcessorsCount);
			ImGui::Text("Memory: %iMB", m_HostInfo.MemorySize / 1024 / 1024);

			ImGui::End();
		}
	}
}