
#include "DX_ImguiManager.h"
#include "DX_Device.h"

#include "../imgui/imgui_impl_dx12.h"
#include "../imgui/imgui_impl_glfw.h"

namespace CGE
{
	namespace DX12
	{
		void DX_ImguiManager::InitInternal(RHI::Device& device)
		{
			const auto& dxDevice = static_cast<const DX_Device&>(device);
			const D3D12_DESCRIPTOR_HEAP_DESC desc =
			{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
			};
			dxDevice.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imguiHeap));

			// init imgui dx12 impl
			ImGui_ImplDX12_Init(dxDevice.GetDevice(), RHI::Limits::Device::FrameCountMax, DXGI_FORMAT_R8G8B8A8_UNORM,
				m_imguiHeap.Get(),
				m_imguiHeap->GetCPUDescriptorHandleForHeapStart(),
				m_imguiHeap->GetGPUDescriptorHandleForHeapStart()
			);
		}

		void DX_ImguiManager::FrameBeginInternal()
		{
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			for (auto& spawnable : m_spawnableWindows)
			{
				spawnable();
			}
		}

		void DX_ImguiManager::BuildCommandListInternal(RHI::CommandList& commandList)
		{
			ImGui::Render();
		}

		void DX_ImguiManager::Render(ID3D12GraphicsCommandList* dxCommandList)
		{
			ImGui::Render();
			ID3D12DescriptorHeap* descriptorHeaps[] = { m_imguiHeap.Get() };
			dxCommandList->SetDescriptorHeaps(1, descriptorHeaps);
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommandList);
		}
	}
}