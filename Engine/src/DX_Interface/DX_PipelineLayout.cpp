
// DX12
#include "DX_PipelineLayout.h"
#include "DX_Conversions.h"
#include "DX_Device.h"

// RHI
#include "../RHI/Limits.h"

namespace CGE
{
	namespace DX12
	{
		DX_PipelineLayout::DX_PipelineLayout(DX_PipelineLayoutCache& parentCache) : m_parentCache{ &parentCache } {}

		DX_PipelineLayout::~DX_PipelineLayout()
		{
			m_rootSignature = nullptr;
		}

		void DX_PipelineLayout::add_ref() const
		{
			assert(m_useCount >= 0, "PipelineLayout has been deleted");
			++m_useCount;
		}

		void DX_PipelineLayout::release() const
		{
			assert(m_useCount > 0, "Usecount is already 0!");
			if (m_useCount.fetch_sub(1) == 1)
			{
				// The cache has ownership.
				if (m_parentCache)
				{
					m_parentCache->TryReleasePipelineLayout(this);
				}

				// We were orphaned by the cache, just delete.
				else
				{
					delete this;
				}
			}
		}

		void DX_PipelineLayout::Init(ID3D12DeviceX* dx12Device, const RHI::PipelineLayoutDescriptor& descriptor, const std::string& name)
		{
			m_hash = descriptor.GetHash();
			// Number of avalible srgs in the descriptor.
			const uint32_t groupLayoutCount = static_cast<uint32_t>(descriptor.GetShaderResourceGroupLayoutCount());
			assert(groupLayoutCount <= RHI::Limits::Pipeline::ShaderResourceGroupCountMax, "Exceeded ShaderResourceGroupLayout count limit.");

			// Each srg will have a one resource, unbounded and dynamic sampler table.
			std::vector<D3D12_ROOT_PARAMETER> parameters;
			std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges[RHI::Limits::Pipeline::ShaderResourceGroupCountMax];
			std::vector<D3D12_DESCRIPTOR_RANGE> unboundedArraydescriptorRanges[RHI::Limits::Pipeline::ShaderResourceGroupCountMax];
			std::vector<D3D12_DESCRIPTOR_RANGE> samplerDescriptorRanges[RHI::Limits::Pipeline::ShaderResourceGroupCountMax];
			std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

			m_layoutDescriptor = &descriptor;
			const DX_PipelineLayoutDescriptor* dx12Descriptor = dynamic_cast<const DX_PipelineLayoutDescriptor*>(&descriptor);
			assert(dx12Descriptor, "Trying to create a pipeline layout without a DX12 pipeline layout descriptor.");

			// [todo] root constant bindings
			// Atom is setting the layout a bit different that me what Atom does for the parameters if avalible:
			//
			// 1 : D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS ( only one root 32 bit constant it will spawn bytes into this from the inline constants in the shader file )
			// 2 : D3D12_ROOT_PARAMETER_TYPE_CBV ( one for each srg if avalibe )
			// 3 : D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE ( one type table of continuous descriptors. Builds the ranges first for buffers and then images )
			// 4 : D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE ( one type table for unbounded ranges. Builds the ranges first for buffers and then images )
			// 5 : D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE ( one type table for dynamic samplers )
			// 6 : At the end sets the static samplers directly in the rootSignatureDesc.
			// 
			// What I'm doing (will change later) is looping through the srg's and:
			// 1 : D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS ( one for each srg if avalibe )
			// 2 : D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE ( one type table of continuous descriptors. Builds the ranges first for buffers and then images )
			// 3 : D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE ( Unbounded tables I will just set these for the bindless tables SrgSlotType::Bindless and this slot wont have anything else )
			// 4 : D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE ( one type table for dynamic samplers )
			// 5 : At the end sets the static samplers directly in the rootSignatureDesc.

			// The slot index will be the srg slot so for now we will have {Object = 0, Pass = 1, Scene = 2, Bindless = 3}
			// The index will be the order to which we set the ShaderResourceGroup's in the ShaderResourceGroupLayout.
			// So lets say say the first index we pushed the Bindless slot first the index zero in m_indexToSlotTable will be 3 (for Bindless).
			// 
			//	 ShaderResourceGroupLayout::m_shaderResourceGroupLayoutsInfo
			//        idx : 0			     idx : 1				idx : 2
			// |---------------------------------------------------------------|
			// |	Bindless : 3	 |  	Object : 0	    |	  Pass : 1     |
			// |---------------------------------------------------------------|
			//
			//							m_slotToIndexTable
			//		Object : 0		Pass : 1		Scene : 2		Bindless : 3
			// |------------------------------------------------------------------|
			// |	  1		  |	      2	     |	 4 (dont have)   |       0	  	  |
			// |------------------------------------------------------------------|
			// 
			//							m_indexToSlotTable
			//		  0				        1				2	
			// |----------------------------------------------------|
			// |	Bindless : 3	|	Object : 0	|	Pass : 1	|
			// |----------------------------------------------------|
			// 
			//			m_indexToRootParameterBindingTable
			//		  0				        1				2	
			// |----------------------------------------------------|
			// |	Bindless : 3	|	Object : 0	|	Pass : 1	|
			// |----------------------------------------------------|

			m_slotToIndexTable.fill(static_cast<uint8_t>(RHI::Limits::Pipeline::ShaderResourceGroupCountMax));
			m_indexToRootParameterBindingTable.resize(groupLayoutCount);
			m_indexToSlotTable.resize(groupLayoutCount);

			for (uint32_t groupLayoutIndex = 0; groupLayoutIndex < groupLayoutCount; ++groupLayoutIndex)
			{
				const RHI::ShaderResourceGroupLayout& groupLayout = *descriptor.GetShaderResourceGroupLayout(groupLayoutIndex);
				const uint32_t srgLayoutSlot = groupLayout.GetBindingSlot();
				m_slotToIndexTable[srgLayoutSlot] = static_cast<uint8_t>(groupLayoutIndex);
				m_indexToSlotTable[groupLayoutIndex] = static_cast<uint8_t>(srgLayoutSlot);
			}

			// Construct a list of indexes sorted by frequency. (most to least)
			// m_slotToIndexTable will be sorted by index if srg is avalible, if not it will be the set to RHI::Limits::Pipeline::ShaderResourceGroupCountMax.
			std::vector<uint8_t> indexesSortedByFrequency(groupLayoutCount);
			uint32_t usedSlotIndex = 0;
			for (uint32_t slot = 0; slot < m_slotToIndexTable.size(); ++slot)
			{
				const uint8_t groupLayoutIndex = m_slotToIndexTable[slot];
				if (groupLayoutIndex != RHI::Limits::Pipeline::ShaderResourceGroupCountMax)
				{
					indexesSortedByFrequency[usedSlotIndex] = groupLayoutIndex;
					++usedSlotIndex;
				}
			}
			assert(usedSlotIndex == groupLayoutCount, "Unexpected number of used slots");

			// Next, process the remaining descriptor tables by frequency.
			for (const uint32_t groupLayoutIndex : indexesSortedByFrequency)
			{
				const RHI::ShaderResourceGroupLayout& groupLayout = *descriptor.GetShaderResourceGroupLayout(groupLayoutIndex);
				const RHI::ShaderResourceGroupBindingInfo& groupBindInfo = dx12Descriptor->GetShaderResourceGroupBindingInfo(groupLayoutIndex);
				// [todo]
				// const DX_ShaderResourceGroupVisibility& groupVisibility = dx12Descriptor->GetShaderResourceGroupVisibility(groupLayoutIndex);

				// First see if the group has a root constant
				// [todo] Atom will spawn a range of bytes in one root constant binding as the first root parameter.
				// I think this will allow easier updates since root constants get updated more often.
				// I will not do that for now and I will have one root constant per srg.
				if (groupLayout.GetShaderInputConstant().has_value())
				{
					m_indexToRootParameterBindingTable[groupLayoutIndex].m_rootConstant = DX_RootParameterBinding::RootParameterIndex(parameters.size());
					parameters.emplace_back();
					D3D12_ROOT_PARAMETER& parameter = parameters.back();

					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					// The number of constants that occupy a single shader slot (these constants appear like a single constant buffer).
					// All constants occupy a single root signature bind slot.
					parameter.Constants.Num32BitValues = groupLayout.GetShaderInputConstant()->m_constantByteCount / 4;
					parameter.Constants.ShaderRegister = groupLayout.GetShaderInputConstant()->m_registerId;
					parameter.Constants.RegisterSpace = groupLayout.GetShaderInputConstant()->m_spaceId;
					parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				}

				if (groupLayout.GetGroupSizeForBuffers() || groupLayout.GetGroupSizeForImages())
				{
					for (const RHI::ShaderInputBufferDescriptor& shaderInputBuffer : groupLayout.GetShaderInputListForBuffers())
					{
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderInputBuffer.m_spaceId;
						descriptorRange.NumDescriptors = shaderInputBuffer.m_count;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
						descriptorRange.BaseShaderRegister = shaderInputBuffer.m_registerId;

						switch (shaderInputBuffer.m_access)
						{
						case RHI::ShaderInputBufferAccess::Constant:
							descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
							break;

						case RHI::ShaderInputBufferAccess::Read:
							descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
							break;

						case RHI::ShaderInputBufferAccess::ReadWrite:
							descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
							break;
						}
						descriptorRanges[groupLayoutIndex].push_back(descriptorRange);
					}

					for (const RHI::ShaderInputImageDescriptor& shaderInputImage : groupLayout.GetShaderInputListForImages())
					{
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderInputImage.m_spaceId;
						descriptorRange.NumDescriptors = shaderInputImage.m_count;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
						descriptorRange.BaseShaderRegister = shaderInputImage.m_registerId;

						switch (shaderInputImage.m_access)
						{
						case RHI::ShaderInputImageAccess::Read:
							descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
							break;

						case RHI::ShaderInputImageAccess::ReadWrite:
							descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
							break;
						}
						descriptorRanges[groupLayoutIndex].push_back(descriptorRange);
					}

					D3D12_ROOT_PARAMETER parameter;
					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					// [todo]
					// parameter.ShaderVisibility = ConvertShaderStageMask(groupVisibility.m_descriptorTableShaderStageMask);
					parameter.ShaderVisibility = ConvertShaderStageMask(RHI::ShaderStageMask::All);
					parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorRanges[groupLayoutIndex].size());
					parameter.DescriptorTable.pDescriptorRanges = descriptorRanges[groupLayoutIndex].data();

					m_indexToRootParameterBindingTable[groupLayoutIndex].m_resourceTable = DX_RootParameterBinding::RootParameterIndex(parameters.size());
					parameters.push_back(parameter);
				}

				// Unbounded tables (Just set these for SrgSlotType::Bindless)
				for (const RHI::ShaderInputBufferUnboundedArrayDescriptor& shaderInputBufferUnboundedArray : groupLayout.GetShaderInputListForBufferUnboundedArrays())
				{
					D3D12_DESCRIPTOR_RANGE descriptorRange;
					descriptorRange.RegisterSpace = shaderInputBufferUnboundedArray.m_spaceId;
					descriptorRange.NumDescriptors = static_cast<UINT>(-1);
					descriptorRange.OffsetInDescriptorsFromTableStart = 0;
					descriptorRange.BaseShaderRegister = shaderInputBufferUnboundedArray.m_registerId;

					switch (shaderInputBufferUnboundedArray.m_access)
					{
					case RHI::ShaderInputBufferAccess::Read:
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						break;

					case RHI::ShaderInputBufferAccess::ReadWrite:
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						break;
					}
					unboundedArraydescriptorRanges[groupLayoutIndex].push_back(descriptorRange);
				}
				for (const RHI::ShaderInputImageUnboundedArrayDescriptor& shaderInputImageUnboundedArray : groupLayout.GetShaderInputListForImageUnboundedArrays())
				{
					D3D12_DESCRIPTOR_RANGE descriptorRange;
					descriptorRange.RegisterSpace = shaderInputImageUnboundedArray.m_spaceId;
					descriptorRange.NumDescriptors = static_cast<UINT>(-1);
					descriptorRange.OffsetInDescriptorsFromTableStart = 0;
					descriptorRange.BaseShaderRegister = shaderInputImageUnboundedArray.m_registerId;

					switch (shaderInputImageUnboundedArray.m_access)
					{
					case RHI::ShaderInputImageAccess::Read:
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						break;

					case RHI::ShaderInputImageAccess::ReadWrite:
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						break;
					}
					unboundedArraydescriptorRanges[groupLayoutIndex].push_back(descriptorRange);
				}
				if (!unboundedArraydescriptorRanges[groupLayoutIndex].empty())
				{
					D3D12_ROOT_PARAMETER parameter;
					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
					parameter.DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(unboundedArraydescriptorRanges[groupLayoutIndex].size());
					parameter.DescriptorTable.pDescriptorRanges = unboundedArraydescriptorRanges[groupLayoutIndex].data();

					m_indexToRootParameterBindingTable[groupLayoutIndex].m_bindlessTable = DX_RootParameterBinding::RootParameterIndex(parameters.size());
					parameters.push_back(parameter);
				}
			}

			// Next, process the dynamic sampler descriptor tables by frequency. Sampler can't be mixed with other resources
			for (const uint32_t groupLayoutIndex : indexesSortedByFrequency)
			{
				const RHI::ShaderResourceGroupLayout& groupLayout = *descriptor.GetShaderResourceGroupLayout(groupLayoutIndex);
				const RHI::ShaderResourceGroupBindingInfo& groupBindInfo = dx12Descriptor->GetShaderResourceGroupBindingInfo(groupLayoutIndex);

				// [todo] Set the visibility
				// const DX_ShaderResourceGroupVisibility& groupVisibility = dx12Descriptor->GetShaderResourceGroupVisibility(groupLayoutIndex);

				if (groupLayout.GetGroupSizeForSamplers())
				{
					for (const RHI::ShaderInputSamplerDescriptor& shaderInputSampler : groupLayout.GetShaderInputListForSamplers())
					{
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderInputSampler.m_spaceId;
						descriptorRange.NumDescriptors = shaderInputSampler.m_count;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						descriptorRange.BaseShaderRegister = shaderInputSampler.m_registerId;
						samplerDescriptorRanges[groupLayoutIndex].push_back(descriptorRange);
					}

					D3D12_ROOT_PARAMETER parameter;
					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					// [todo]
					// parameter.ShaderVisibility = ConvertShaderStageMask(groupVisibility.m_descriptorTableShaderStageMask);
					parameter.ShaderVisibility = ConvertShaderStageMask(RHI::ShaderStageMask::All);
					parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(samplerDescriptorRanges[groupLayoutIndex].size());
					parameter.DescriptorTable.pDescriptorRanges = samplerDescriptorRanges[groupLayoutIndex].data();

					m_indexToRootParameterBindingTable[groupLayoutIndex].m_samplerTable = DX_RootParameterBinding::RootParameterIndex(parameters.size());
					parameters.push_back(parameter);
				}
			}

			// Last, process by frequency the static samplers
			for (const uint32_t groupLayoutIndex : indexesSortedByFrequency)
			{
				const RHI::ShaderResourceGroupLayout& groupLayout = *descriptor.GetShaderResourceGroupLayout(groupLayoutIndex);
				const RHI::ShaderResourceGroupBindingInfo& groupBindInfo = dx12Descriptor->GetShaderResourceGroupBindingInfo(groupLayoutIndex);
				// [todo]
				// const DX_ShaderResourceGroupVisibility& groupVisibility = dx12Descriptor->GetShaderResourceGroupVisibility(groupLayoutIndex);

				for (const RHI::ShaderInputStaticSamplerDescriptor& samplerInput : groupLayout.GetStaticSamplers())
				{
					D3D12_STATIC_SAMPLER_DESC desc;
					// [todo]
					// ConvertStaticSampler(samplerInput.m_samplerState, samplerInput.m_registerId, samplerInput.m_spaceId, ConvertShaderStageMask(groupVisibility.m_descriptorTableShaderStageMask), desc);
					ConvertStaticSampler(samplerInput.m_samplerState, samplerInput.m_registerId, samplerInput.m_spaceId, ConvertShaderStageMask(RHI::ShaderStageMask::All), desc);
					staticSamplers.push_back(desc);
				}
			}

			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSignatureDesc.NumParameters = static_cast<uint32_t>(parameters.size());
			rootSignatureDesc.pParameters = parameters.data();
			rootSignatureDesc.NumStaticSamplers = static_cast<uint32_t>(staticSamplers.size());
			rootSignatureDesc.pStaticSamplers = staticSamplers.data();

			Microsoft::WRL::ComPtr<ID3DBlob> pOutBlob, pErrorBlob;
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf());
			if (pErrorBlob)
			{
				std::cout << reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()) << std::endl;
			}
			assert(pOutBlob, "Failed to serialize root signature.");

			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
			DXAssertSuccess(dx12Device->CreateRootSignature(1, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf())));
			m_rootSignature = rootSignature.Get();
			m_rootSignature->SetName(s2ws(name).c_str());
		}

		ID3D12RootSignature* DX_PipelineLayout::Get() const
		{
			return m_rootSignature.get();
		}

		const RHI::PipelineLayoutDescriptor& DX_PipelineLayout::GetPipelineLayoutDescriptor() const
		{
			return *m_layoutDescriptor;
		}

		RHI::HashValue64 DX_PipelineLayout::GetHash() const
		{
			return m_hash;
		}

		size_t DX_PipelineLayout::GetSlotByIndex(size_t index) const
		{
			return m_indexToSlotTable[index];
		}

		size_t DX_PipelineLayout::GetIndexBySlot(size_t slot) const
		{
			return m_slotToIndexTable[slot];
		}

		size_t DX_PipelineLayout::GetRootParameterBindingCount() const
		{
			return m_indexToRootParameterBindingTable.size();
		}

		DX_RootParameterBinding DX_PipelineLayout::GetRootParameterBindingByIndex(size_t index) const
		{
			return m_indexToRootParameterBindingTable[index];
		}

		void DX_PipelineLayoutCache::Init(DX_Device& device)
		{
			m_parentDevice = &device;
		}

		void DX_PipelineLayoutCache::Shutdown()
		{
			for (auto& keyValue : m_pipelineLayouts)
			{
				keyValue.second->m_parentCache = nullptr;
			}
			m_pipelineLayouts.clear();
			m_parentDevice = nullptr;
		}

		RHI::ConstPtr<DX_PipelineLayout> DX_PipelineLayoutCache::Allocate(const RHI::PipelineLayoutDescriptor& descriptor)
		{
			assert(m_parentDevice, "Cache is not initialized.");
			const uint64_t hashCode = static_cast<uint64_t>(descriptor.GetHash());
			bool isFirstCompile = false;
			RHI::Ptr<DX_PipelineLayout> layout;

			{
				auto iter = m_pipelineLayouts.find(hashCode);
				// Reserve space so the next inquiry will find that someone got here first.
				if (iter == m_pipelineLayouts.end())
				{
					iter = m_pipelineLayouts.emplace(hashCode, new DX_PipelineLayout(*this)).first;
					isFirstCompile = true;
				}
				layout = iter->second;
			}

			if (isFirstCompile)
			{
				layout->Init(m_parentDevice->GetDevice(), descriptor, descriptor.GetName());
			}
			return layout;
		}

		void DX_PipelineLayoutCache::TryReleasePipelineLayout(const DX_PipelineLayout* pipelineLayout)
		{
			if (!pipelineLayout)
			{
				return;
			}
			int32_t expectedRefCount = 0;
			if (pipelineLayout->m_useCount == expectedRefCount)
			{
				auto iter = m_pipelineLayouts.find(static_cast<uint64_t>(pipelineLayout->GetHash()));
				if (iter != m_pipelineLayouts.end())
				{
					m_pipelineLayouts.erase(iter);
				}
				m_parentDevice->QueueForRelease(pipelineLayout->m_rootSignature);
				delete pipelineLayout;
			}
		}
	}
}