#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_PipelineLayoutDescriptor.h"

// RHI
#include "../RHI/ShaderResourceGroupLayout.h"

#include <d3d12.h>

namespace CGE
{
	namespace DX12
	{
		class DX_Device;
		class DX_PipelineLayoutCache;

		class DX_PipelineLayout
		{
			friend class DX_PipelineLayoutCache;
		public:
			DX_PipelineLayout() = default;
			~DX_PipelineLayout();
			void Init(ID3D12DeviceX* dx12Device, const RHI::PipelineLayoutDescriptor& descriptor, const std::string& name);
			ID3D12RootSignature* Get() const;
			const RHI::PipelineLayoutDescriptor& GetPipelineLayoutDescriptor() const;
			RHI::HashValue64 GetHash() const;

			// Returns the SRG binding slot associated with the SRG flat index.
			size_t GetSlotByIndex(size_t index) const;
			// Returns the SRG flat index associated with the SRG binding slot.
			size_t GetIndexBySlot(size_t slot) const;
			// Returns the number of root parameter bindings (1-to-1 with SRG).
			size_t GetRootParameterBindingCount() const;
			// Returns the root parameter binding for the flat index (1-to-1 with SRG).
			DX_RootParameterBinding GetRootParameterBindingByIndex(size_t index) const;
		private:
			DX_PipelineLayout(DX_PipelineLayoutCache& parentPool);

			template <typename T>
			friend struct IntrusivePtrCountPolicy;
			void add_ref() const;
			void release() const;
		
		private:
			std::string m_name;

			// The pipeline layout descriptor can have srg slots which are sparse (not ordered)
			// Each entry index in m_slotToIndexTable will correspond to a srg type sorted from object to scene for now.
			// Each entry will correspond to the pipeline layout descriptor enties in its array.
			// So if we have PipelineLayoutDescriptor =>[ 0 : [SrgType = SceneSrg : SrgIndex = 2], 1 : [ SrgType = ObjectSrg : SrgIndex = 0] ]
			// Then m_slotToIndexTable will be [ 1, 3, 0 ] So at index two which is SrgType::SceneSrg we will have 0 idx
			// which is the corresponding idx in the pipeline layout descriptor enties.
			std::array<uint8_t, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> m_slotToIndexTable;
			std::vector<uint8_t> m_indexToSlotTable;

			// Table for mapping SRG index (packed) to Root Parameter Binding (DX12 command list bindings).
			// This table is used pull the slot bindings dictated by the pipeline layout. (check DX_CommandList::CommitShaderResources)
			std::vector<DX_RootParameterBinding> m_indexToRootParameterBindingTable;

			RHI::Ptr<ID3D12RootSignature> m_rootSignature;
			RHI::ConstPtr<RHI::PipelineLayoutDescriptor> m_layoutDescriptor;
			std::vector<RHI::Ptr<RHI::ShaderResourceGroupLayout>> m_sortedSrgLayouts;
			RHI::HashValue64 m_hash = RHI::HashValue64{ 0 };
			DX_PipelineLayoutCache* m_parentCache = nullptr;
			mutable std::atomic_int m_useCount = { 0 };
		};

		class DX_PipelineLayoutCache final
		{
			friend class DX_PipelineLayout;
		public:
			DX_PipelineLayoutCache() = default;
			void Init(DX_Device& device);
			void Shutdown();

			// Allocates an instance of a pipeline layout from a descriptor and adds to registery.
			RHI::ConstPtr<DX_PipelineLayout> Allocate(const RHI::PipelineLayoutDescriptor& descriptor);

		private:
			void TryReleasePipelineLayout(const DX_PipelineLayout* pipelineLayout);

			std::unordered_map<uint64_t, DX_PipelineLayout*> m_pipelineLayouts;
			DX_Device* m_parentDevice = nullptr;
		};
	}
}