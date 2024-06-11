#pragma once

#include "../ImguiManager.h"

namespace CGE
{
	namespace DX12
	{
		class DX_ImguiManager : public ImguiManager
		{
		public:
			DX_ImguiManager() = default;
			
			// [todo] remove later
			void Render(ID3D12GraphicsCommandList* dxCommandList);
		private:
			void InitInternal(RHI::Device& device) override;
			void FrameBeginInternal() override;
			void BuildCommandListInternal(RHI::CommandList& commandList) override;

		private:
			// exclusive heap used by imgui (for the dx12 backend)
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_imguiHeap;
		};
	}
}