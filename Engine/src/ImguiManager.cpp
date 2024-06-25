
#include "ImguiManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

namespace CGE
{
	void ImguiManager::Init(RHI::Device& device)
	{
		RHI::DeviceObject::Init(device);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui::GetStyle().ScaleAllSizes(1.5f);
		ImGui::GetIO().FontGlobalScale = 1.4f;
		InitInternal(device);
	}

	void ImguiManager::Shutdown()
	{
		ImGui::DestroyContext();
	}

	void ImguiManager::FrameBegin()
	{
		FrameBeginInternal();
	}

	void ImguiManager::BuildCommandList(RHI::CommandList& commandList)
	{
		BuildCommandListInternal(commandList);
	}

	void ImguiManager::PushSpawnableWindow(std::function<void()> spawnableWindow)
	{
		m_spawnableWindows.push_back(spawnableWindow);
	}
}