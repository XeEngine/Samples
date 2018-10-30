#include "pch.h"
#include "FrameHelloWorld.h"
#include <imgui/imgui.h>

namespace ImGuiSample
{
	void FrameHelloWorld::operator()(const Xe::Game::ImGuiFrameArgs& args)
	{
		if (ImGui::Begin("Hello world frame"))
		{
			m_ForeHsvColor.h = Xe::Math::Fmod(m_ForeHsvColor.h + 1.0f, 360.0f);
			m_ForeHsvColor.s = 1.0f;
			m_ForeHsvColor.v = 1.0f;
			m_ForeHsvColor.a = 1.0f;

			auto color = Xe::Graphics::Color::FromHsv(m_ForeHsvColor);
			ImVec4 imColor{ color.b, color.g, color.r, color.a };

			ImGui::TextColored(imColor, "Hello world from XeEngine!");

			ImGui::End();
		}
	}
}