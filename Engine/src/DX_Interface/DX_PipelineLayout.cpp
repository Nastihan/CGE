
// DX12
#include "DX_PipelineLayout.h"
#include "DX_Conversions.h"

// RHI
#include "../RHI/Limits.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_PipelineLayout> DX_PipelineLayout::Create()
		{
			return new DX_PipelineLayout();
		}

		void DX_PipelineLayout::Init(ID3D12DeviceX* dx12Device, const std::vector<RHI::Ptr<RHI::ShaderResourceGroupLayout>>& srgLayouts, const std::string& name)
		{
			std::vector<D3D12_ROOT_PARAMETER> parameters;
			std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges[RHI::Limits::Pipeline::ShaderResourceGroupCountMax];
			std::vector<D3D12_DESCRIPTOR_RANGE> unboundedArraydescriptorRanges[RHI::Limits::Pipeline::ShaderResourceGroupCountMax];
			std::vector<D3D12_DESCRIPTOR_RANGE> samplerDescriptorRanges[RHI::Limits::Pipeline::ShaderResourceGroupCountMax];
			std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

			for (size_t groupLayoutIndex = 0; groupLayoutIndex < srgLayouts.size(); groupLayoutIndex++)
			{
				const auto& rootConstantBinding = srgLayouts[groupLayoutIndex]->GetRootConstant();
				if (rootConstantBinding.has_value())
				{
					parameters.emplace_back();
					D3D12_ROOT_PARAMETER& parameter = parameters.back();
					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					parameter.Constants.Num32BitValues = rootConstantBinding->m_constantCount;
					parameter.Constants.ShaderRegister = rootConstantBinding->m_constantRegister;
					parameter.Constants.RegisterSpace = rootConstantBinding->m_constantRegisterSpace;
					parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				}
			}

			for (size_t groupLayoutIndex = 0; groupLayoutIndex < srgLayouts.size(); groupLayoutIndex++)
			{
				if (srgLayouts[groupLayoutIndex]->GetShaderInputListForBuffers().size() || srgLayouts[groupLayoutIndex]->GetShaderInputListForImages().size() || srgLayouts[groupLayoutIndex]->GetShaderInputListForConstants().size())
				{
					// [todo] This is bad change later.
					RHI::ShaderStageMask m_descriptorTableShaderStageMask = RHI::ShaderStageMask::All;
					for (const RHI::ShaderInputBufferDescriptor& shaderInputBuffer : srgLayouts[groupLayoutIndex]->GetShaderInputListForBuffers())
					{
						m_descriptorTableShaderStageMask = shaderInputBuffer.m_resourceBindingInfo.m_shaderStageMask;
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderInputBuffer.m_resourceBindingInfo.m_spaceId;
						descriptorRange.NumDescriptors = shaderInputBuffer.m_count;
						// D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND indicates this range should immediately follow the preceding range.
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
						descriptorRange.BaseShaderRegister = shaderInputBuffer.m_resourceBindingInfo.m_registerId;

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
					for (const RHI::ShaderInputImageDescriptor& shaderInputImage : srgLayouts[groupLayoutIndex]->GetShaderInputListForImages())
					{
						m_descriptorTableShaderStageMask = shaderInputImage.m_resourceBindingInfo.m_shaderStageMask;
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderInputImage.m_resourceBindingInfo.m_spaceId;
						descriptorRange.NumDescriptors = shaderInputImage.m_count;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
						descriptorRange.BaseShaderRegister = shaderInputImage.m_resourceBindingInfo.m_registerId;

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
					for (const RHI::ShaderInputConstantDescriptor& shaderConstantBuffer : srgLayouts[groupLayoutIndex]->GetShaderInputListForConstants())
					{
						m_descriptorTableShaderStageMask = shaderConstantBuffer.m_resourceBindingInfo.m_shaderStageMask;
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderConstantBuffer.m_resourceBindingInfo.m_spaceId;
						descriptorRange.NumDescriptors = shaderConstantBuffer.m_count;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
						descriptorRange.BaseShaderRegister = shaderConstantBuffer.m_resourceBindingInfo.m_registerId;
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
						descriptorRanges[groupLayoutIndex].push_back(descriptorRange);
					}
					D3D12_ROOT_PARAMETER parameter;
					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					parameter.ShaderVisibility = ConvertShaderStageMask(m_descriptorTableShaderStageMask);
					parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorRanges[groupLayoutIndex].size());
					parameter.DescriptorTable.pDescriptorRanges = descriptorRanges[groupLayoutIndex].data();
					parameters.push_back(parameter);
				}

				for (const RHI::ShaderInputBufferUnboundedArrayDescriptor& shaderInputBufferUnboundedArray : srgLayouts[groupLayoutIndex]->GetShaderInputListForBufferUnboundedArrays())
				{
					D3D12_DESCRIPTOR_RANGE descriptorRange;
					descriptorRange.RegisterSpace = shaderInputBufferUnboundedArray.m_resourceBindingInfo.m_spaceId;
					descriptorRange.NumDescriptors = static_cast<UINT>(-1);
					descriptorRange.OffsetInDescriptorsFromTableStart = 0;
					descriptorRange.BaseShaderRegister = shaderInputBufferUnboundedArray.m_resourceBindingInfo.m_registerId;

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

				for (const RHI::ShaderInputImageUnboundedArrayDescriptor& shaderInputImageUnboundedArray : srgLayouts[groupLayoutIndex]->GetShaderInputListForImageUnboundedArrays())
				{
					D3D12_DESCRIPTOR_RANGE descriptorRange;
					descriptorRange.RegisterSpace = shaderInputImageUnboundedArray.m_resourceBindingInfo.m_spaceId;
					descriptorRange.NumDescriptors = static_cast<UINT>(-1);
					descriptorRange.OffsetInDescriptorsFromTableStart = 0;
					descriptorRange.BaseShaderRegister = shaderInputImageUnboundedArray.m_resourceBindingInfo.m_registerId;

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
					parameters.push_back(parameter);
				}
			}

			for (size_t groupLayoutIndex = 0; groupLayoutIndex < srgLayouts.size(); groupLayoutIndex++)
			{
				if (srgLayouts[groupLayoutIndex]->GetShaderInputListForSamplers().size())
				{
					// [todo] This is bad change later.
					RHI::ShaderStageMask m_descriptorTableShaderStageMask = RHI::ShaderStageMask::All;
					for (const RHI::ShaderInputSamplerDescriptor& shaderInputSampler : srgLayouts[groupLayoutIndex]->GetShaderInputListForSamplers())
					{
						m_descriptorTableShaderStageMask = shaderInputSampler.m_resourceBindingInfo.m_shaderStageMask;
						D3D12_DESCRIPTOR_RANGE descriptorRange;
						descriptorRange.RegisterSpace = shaderInputSampler.m_resourceBindingInfo.m_spaceId;
						descriptorRange.NumDescriptors = shaderInputSampler.m_count;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						descriptorRange.BaseShaderRegister = shaderInputSampler.m_resourceBindingInfo.m_registerId;
						samplerDescriptorRanges[groupLayoutIndex].push_back(descriptorRange);
					}

					D3D12_ROOT_PARAMETER parameter;
					parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					parameter.ShaderVisibility = ConvertShaderStageMask(m_descriptorTableShaderStageMask);
					parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(samplerDescriptorRanges[groupLayoutIndex].size());
					parameter.DescriptorTable.pDescriptorRanges = samplerDescriptorRanges[groupLayoutIndex].data();
					parameters.push_back(parameter);
				}
			}

			for (size_t groupLayoutIndex = 0; groupLayoutIndex < srgLayouts.size(); groupLayoutIndex++)
			{
				for (const RHI::ShaderInputStaticSamplerDescriptor& samplerInput : srgLayouts[groupLayoutIndex]->GetStaticSamplers())
				{
					D3D12_STATIC_SAMPLER_DESC desc;
					ConvertStaticSampler(samplerInput.m_samplerState, samplerInput.m_resourceBindingInfo.m_registerId, samplerInput.m_resourceBindingInfo.m_spaceId, ConvertShaderStageMask(samplerInput.m_resourceBindingInfo.m_shaderStageMask), desc);
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
	}
}