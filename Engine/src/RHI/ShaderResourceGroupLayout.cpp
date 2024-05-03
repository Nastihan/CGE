
// RHI
#include "ShaderResourceGroupLayout.h"

namespace CGE
{
	namespace RHI
	{
		RHI::Ptr<ShaderResourceGroupLayout> ShaderResourceGroupLayout::Create()
		{
			return new ShaderResourceGroupLayout();
		}

		void ShaderResourceGroupLayout::Clear()
		{
			m_staticSamplers.clear();
			m_inputsForBuffers.clear();
			m_inputsForImages.clear();
			m_inputsForSamplers.clear();
			m_inputsForBufferUnboundedArrays.clear();
			m_inputsForImageUnboundedArrays.clear();
		}

		void ShaderResourceGroupLayout::SetShaderResourceGroupType(ShaderResourceGroupType type)
		{
			m_srgType = type;
		}

		ShaderResourceGroupType ShaderResourceGroupLayout::GetShaderResourceGroupType() const
		{
			return m_srgType;
		}

		void ShaderResourceGroupLayout::AddStaticSampler(const ShaderInputStaticSamplerDescriptor& sampler)
		{
			m_staticSamplers.push_back(sampler);
		}

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputBufferDescriptor& buffer)
		{
			m_inputsForBuffers.push_back(buffer);
		}

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputImageDescriptor& image)
		{
			m_inputsForImages.push_back(image);
		}

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputBufferUnboundedArrayDescriptor& bufferUnboundedArray)
		{
			m_inputsForBufferUnboundedArrays.push_back(bufferUnboundedArray);
		}

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputImageUnboundedArrayDescriptor& imageUnboundedArray)
		{
			m_inputsForImageUnboundedArrays.push_back(imageUnboundedArray);
		}

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputSamplerDescriptor& sampler)
		{
			m_inputsForSamplers.push_back(sampler);
		}

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputConstantDescriptor& constant)
		{
			// m_constantsDataLayout->AddShaderInput(constant);
		}

		std::span<const ShaderInputStaticSamplerDescriptor> ShaderResourceGroupLayout::GetStaticSamplers() const
		{
			return m_staticSamplers;
		}

		std::span<const ShaderInputBufferDescriptor> ShaderResourceGroupLayout::GetShaderInputListForBuffers() const
		{
			return m_inputsForBuffers;
		}

		std::span<const ShaderInputImageDescriptor> ShaderResourceGroupLayout::GetShaderInputListForImages() const
		{
			return m_inputsForImages;
		}

		std::span<const ShaderInputSamplerDescriptor> ShaderResourceGroupLayout::GetShaderInputListForSamplers() const
		{
			return m_inputsForSamplers;
		}

		// std::span<const ShaderInputConstantDescriptor> ShaderResourceGroupLayout::GetShaderInputListForConstants() const
		// {
			// return m_constantsDataLayout;
		// }

		std::span<const ShaderInputBufferUnboundedArrayDescriptor> ShaderResourceGroupLayout::GetShaderInputListForBufferUnboundedArrays() const
		{
			return m_inputsForBufferUnboundedArrays;
		}

		std::span<const ShaderInputImageUnboundedArrayDescriptor> ShaderResourceGroupLayout::GetShaderInputListForImageUnboundedArrays() const
		{
			return m_inputsForImageUnboundedArrays;
		}
	}
}