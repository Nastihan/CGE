
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
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				// ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				// ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				// ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				// ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
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