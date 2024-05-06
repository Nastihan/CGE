#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/PipelineLayout.h"
#include "../RHI/ShaderResourceGroupLayout.h"

#include <d3d12.h>

namespace CGE
{
	namespace DX12
	{
		class DX_Device;

		class DX_PipelineLayout : public RHI::PipelineLayout
		{
		public:
			static RHI::Ptr<DX_PipelineLayout> Create();
			void Init(ID3D12DeviceX* dx12Device, const std::vector<RHI::Ptr<RHI::ShaderResourceGroupLayout>>& srgLayouts, const std::string& name);
			ID3D12RootSignature* Get() const;
		private:
			DX_PipelineLayout() = default;
		
		private:
			RHI::Ptr<ID3D12RootSignature> m_rootSignature;
			std::vector<RHI::Ptr<RHI::ShaderResourceGroupLayout>> m_sortedSrgLayouts;
			RHI::HashValue64 m_hash = RHI::HashValue64{ 0 };
		};
	}
}