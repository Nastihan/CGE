#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_DescriptorPool.h"

// std
#include <unordered_map>
#include <array>
#include <string>
#include <optional>

namespace CGE
{
	namespace DX12
	{
		class DX_Device;

		class DX_DescriptorContext
		{
		public:
			DX_DescriptorContext() = default;
			void Init(ID3D12DeviceX* device);

			// [todo] change to take in image view
			void CreateRenderTargetView(ID3D12Resource* backBuffer, DX_DescriptorHandle& rtv);

			D3D12_CPU_DESCRIPTOR_HANDLE GetCpuPlatformHandle(DX_DescriptorHandle handle) const;

		private:
			std::optional<D3D12_DESCRIPTOR_HEAP_TYPE> StringToDescriptorHeapType(std::string heapType);
			bool IsShaderVisibleCbvSrvUavHeap(uint32_t type, uint32_t flag) const;
			DX_DescriptorPool& GetPool(uint32_t type, uint32_t flag);
			const DX_DescriptorPool& GetPool(uint32_t type, uint32_t flag) const;
			// Allocates a single descriptor handle
			DX_DescriptorHandle AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t count);

		private:
			static const uint32_t NumHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE + 1;
			static const uint32_t s_descriptorCountMax[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][NumHeapFlags];
			RHI::Ptr<ID3D12DeviceX> m_device;
			DX_DescriptorPool m_pools[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES][NumHeapFlags];
			std::unordered_map<std::string, std::array<uint32_t, NumHeapFlags>> m_descriptorHeapLimits = std::unordered_map<std::string, std::array<uint32_t, NumHeapFlags>>({
						{ std::string("DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV"), { 100'000, 1'000'000 } },
						{ std::string("DESCRIPTOR_HEAP_TYPE_SAMPLER"), { 2048, 2048 } },
						{ std::string("DESCRIPTOR_HEAP_TYPE_RTV"), { 2048, 0 } },
						{ std::string("DESCRIPTOR_HEAP_TYPE_DSV"), { 2048, 0 } }
				});

			// Descriptors that persist for the lifetime of the resource view they reference
			DX_DescriptorPool m_staticPool;
			// This table binds the entire range of CBV_SRV_UAV descriptor handles in the shader visible heap (used for bindless)
			DX_DescriptorTable m_staticTable;
			// Offset from the shader-visible descriptor heap start to the first static descriptor handle
			uint32_t m_staticDescriptorOffset = 0;
			// Denote portion of the shader-visible descriptor heap used to maintain static handles.
			// NOTE: dynamic descriptors are needed to allocate per-frame descriptor tables for resources that are
			// not bound via bindless, so this number should reflect that. If the majority of resources correctly
			// leverage the bindless mechanism, this number can be higher (e.g. [0.8f, 0.9f]).
			// This can also be calculated based on engine needs.
			float m_staticDescriptorRatio = 0.5f;
		};
	}
}