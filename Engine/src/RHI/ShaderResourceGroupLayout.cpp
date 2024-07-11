
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

		bool ShaderResourceGroupLayout::IsFinalized() const
		{
			return m_hash != HashValue64{ 0 };
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

		bool ShaderResourceGroupLayout::Finalize()
		{
			if (IsFinalized())
			{
				return true;
			}
			if (m_bindingSlot.IsNull() || m_bindingSlot.GetIndex() >= Limits::Pipeline::ShaderResourceGroupCountMax)
			{
				assert(!m_bindingSlot.IsNull(), "You must supply a valid binding slot to ShaderResourceGroupLayoutDescriptor.");
				assert(!(m_bindingSlot.GetIndex() >= Limits::Pipeline::ShaderResourceGroupCountMax));
				Clear();
				return false;
			}

			// Build buffer group
			if (!FinalizeShaderInputGroup<ShaderInputBufferDescriptor, ShaderInputBufferIndex>(m_inputsForBuffers, m_intervalsForBuffers, m_groupSizeForBuffers, m_nameToIdxReflectionForBuffers))
			{
				return false;
			}

			// Build image group
			if (!FinalizeShaderInputGroup<ShaderInputImageDescriptor, ShaderInputImageIndex>(m_inputsForImages, m_intervalsForImages, m_groupSizeForImages, m_nameToIdxReflectionForImages))
			{
				return false;
			}

			// Build buffer unbounded array group
			if (!FinalizeUnboundedArrayShaderInputGroup<ShaderInputBufferUnboundedArrayDescriptor, ShaderInputBufferUnboundedArrayIndex>(m_inputsForBufferUnboundedArrays, m_groupSizeForBufferUnboundedArrays))
			{
				return false;
			}

			// Build image unbounded array group
			if (!FinalizeUnboundedArrayShaderInputGroup<ShaderInputImageUnboundedArrayDescriptor, ShaderInputImageUnboundedArrayIndex>(m_inputsForImageUnboundedArrays, m_groupSizeForImageUnboundedArrays))
			{
				return false;
			}

			// Build sampler group
			if (!FinalizeShaderInputGroup<ShaderInputSamplerDescriptor, ShaderInputSamplerIndex>(m_inputsForSamplers, m_intervalsForSamplers, m_groupSizeForSamplers, m_nameToIdxReflectionForSamplers))
			{
				return false;
			}

			// Build the final hash based on the inputs.
			{
				HashValue64 hash = HashValue64{ 0 };
				for (const ShaderInputStaticSamplerDescriptor& staticSamplerDescriptor : m_staticSamplers)
				{
					hash = staticSamplerDescriptor.GetHash(hash);
				}
				for (const ShaderInputBufferDescriptor& shaderInputBuffer : m_inputsForBuffers)
				{
					hash = shaderInputBuffer.GetHash(hash);
				}
				for (const ShaderInputImageDescriptor& shaderInputImage : m_inputsForImages)
				{
					hash = shaderInputImage.GetHash(hash);
				}
				for (const ShaderInputBufferUnboundedArrayDescriptor& shaderInputBufferUnboundedArray : m_inputsForBufferUnboundedArrays)
				{
					hash = shaderInputBufferUnboundedArray.GetHash(hash);
				}
				for (const ShaderInputImageUnboundedArrayDescriptor& shaderInputImageUnboundedArray : m_inputsForImageUnboundedArrays)
				{
					hash = shaderInputImageUnboundedArray.GetHash(hash);
				}
				for (const ShaderInputSamplerDescriptor& shaderInputSampler : m_inputsForSamplers)
				{
					hash = shaderInputSampler.GetHash(hash);
				}
				hash = TypeHash64(m_bindingSlot.GetIndex(), hash);
				m_hash = hash;
			}
			return true;
		}

		void ShaderResourceGroupLayout::SetName(const std::string& name)
		{
			m_name = name;
		}

		const std::string& ShaderResourceGroupLayout::GetName() const
		{
			return m_name;
		}

		void ShaderResourceGroupLayout::SetShaderResourceGroupType(ShaderResourceGroupType type)
		{
			m_srgType = type;
		}

		ShaderResourceGroupType ShaderResourceGroupLayout::GetShaderResourceGroupType() const
		{
			return m_srgType;
		}

		HashValue64 ShaderResourceGroupLayout::GetHash() const
		{
			assert(IsFinalized());
			return m_hash;
		}

		void ShaderResourceGroupLayout::SetBindingSlot(uint32_t bindingSlot)
		{
			m_bindingSlot = Handle<uint32_t>(bindingSlot);
		}

		uint32_t ShaderResourceGroupLayout::GetBindingSlot() const
		{
			return m_bindingSlot.GetIndex();
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

		void ShaderResourceGroupLayout::AddShaderInput(const ShaderInputConstantDescriptor& inlineConstant)
		{
			m_inputForConstant = inlineConstant;
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

		std::span<const ShaderInputBufferUnboundedArrayDescriptor> ShaderResourceGroupLayout::GetShaderInputListForBufferUnboundedArrays() const
		{
			return m_inputsForBufferUnboundedArrays;
		}

		std::span<const ShaderInputImageUnboundedArrayDescriptor> ShaderResourceGroupLayout::GetShaderInputListForImageUnboundedArrays() const
		{
			return m_inputsForImageUnboundedArrays;
		}

		const std::optional<ShaderInputConstantDescriptor>& ShaderResourceGroupLayout::GetShaderInputConstant() const
		{
			return m_inputForConstant;
		}

		const ShaderInputBufferDescriptor& ShaderResourceGroupLayout::GetShaderInput(ShaderInputBufferIndex index) const
		{
			return m_inputsForBuffers[index.GetIndex()];
		}

		const ShaderInputImageDescriptor& ShaderResourceGroupLayout::GetShaderInput(ShaderInputImageIndex index) const
		{
			return m_inputsForImages[index.GetIndex()];
		}

		const ShaderInputSamplerDescriptor& ShaderResourceGroupLayout::GetShaderInput(ShaderInputSamplerIndex index) const
		{
			return m_inputsForSamplers[index.GetIndex()];
		}

		const std::optional<ShaderInputConstantDescriptor>& ShaderResourceGroupLayout::GetShaderInput() const
		{
			return m_inputForConstant;
		}

		const ShaderInputBufferUnboundedArrayDescriptor& ShaderResourceGroupLayout::GetShaderInput(ShaderInputBufferUnboundedArrayIndex index) const
		{
			return m_inputsForBufferUnboundedArrays[index.GetIndex()];
		}

		const ShaderInputImageUnboundedArrayDescriptor& ShaderResourceGroupLayout::GetShaderInput(ShaderInputImageUnboundedArrayIndex index) const
		{
			return m_inputsForImageUnboundedArrays[index.GetIndex()];
		}

		bool ShaderResourceGroupLayout::ValidateAccess(RHI::ShaderInputBufferIndex inputIndex, uint32_t arrayIndex) const
		{
			return ValidateAccess(inputIndex, arrayIndex, m_inputsForBuffers.size(), "Buffer");
		}

		bool ShaderResourceGroupLayout::ValidateAccess(RHI::ShaderInputImageIndex inputIndex, uint32_t arrayIndex) const
		{
			return ValidateAccess(inputIndex, arrayIndex, m_inputsForImages.size(), "Image");
		}

		bool ShaderResourceGroupLayout::ValidateAccess(RHI::ShaderInputSamplerIndex inputIndex, uint32_t arrayIndex) const
		{
			return ValidateAccess(inputIndex, arrayIndex, m_inputsForSamplers.size(), "Sampler");
		}

		bool ShaderResourceGroupLayout::ValidateAccess(RHI::ShaderInputBufferUnboundedArrayIndex inputIndex) const
		{
			return ValidateAccess(inputIndex, m_inputsForBufferUnboundedArrays.size(), "BufferUnboundedArray");
		}

		bool ShaderResourceGroupLayout::ValidateAccess(RHI::ShaderInputImageUnboundedArrayIndex inputIndex) const
		{
			return ValidateAccess(inputIndex, m_inputsForImageUnboundedArrays.size(), "ImageUnboundedArray");
		}

		template<typename IndexType>
		bool ShaderResourceGroupLayout::ValidateAccess(IndexType inputIndex, uint32_t arrayIndex, size_t inputIndexLimit, const char* inputArrayTypeName) const
		{
			if (!ValidateAccess(inputIndex, inputIndexLimit, inputArrayTypeName))
			{
				return false;
			}
			auto& descriptor = GetShaderInput(inputIndex);
			if (arrayIndex >= descriptor.m_count)
			{
				assert(false, "Invalid input of type " + inputArrayTypeName);
				return false;
			}
			return true;
		}

		template<typename IndexType>
		bool ShaderResourceGroupLayout::ValidateAccess(IndexType inputIndex, size_t inputIndexLimit, const char* inputArrayTypeName) const
		{
			if (inputIndex.GetIndex() >= inputIndexLimit)
			{
				assert(false, "Input count exceeds limit range.");
				return false;
			}
			return true;
		}

		template<typename ShaderInputDescriptorT, typename ShaderInputIndexT>
		bool ShaderResourceGroupLayout::FinalizeShaderInputGroup(const std::vector<ShaderInputDescriptorT>& shaderInputDescriptors, std::vector<Interval>& intervals, uint32_t& groupSize
		, std::unordered_map<std::string, ShaderInputIndexT>& reflectionMap)
		{
			intervals.reserve(shaderInputDescriptors.size());
			reflectionMap.reserve(shaderInputDescriptors.size());

			uint32_t currentGroupSize = 0;
			uint32_t shaderInputIndex = 0;
			for (const ShaderInputDescriptorT& shaderInput : shaderInputDescriptors)
			{
				const ShaderInputIndexT inputIndex(shaderInputIndex);

				// [todo] I have to change this so names will be processed to be unique
				auto insertedResult = reflectionMap.insert({ shaderInput.m_name, inputIndex });
				assert(insertedResult.second, "Make sure to use unique names.");

				intervals.emplace_back(currentGroupSize, currentGroupSize + shaderInput.m_count);
				currentGroupSize += shaderInput.m_count;
				++shaderInputIndex;
			}
			groupSize = currentGroupSize;
			return true;
		}

		template<typename ShaderInputDescriptorT, typename ShaderInputIndexT>
		bool ShaderResourceGroupLayout::FinalizeUnboundedArrayShaderInputGroup(const std::vector<ShaderInputDescriptorT>& shaderInputDescriptors, uint32_t& groupSize)
		{
			uint32_t currentGroupSize = 0;
			uint32_t shaderInputIndex = 0;
			for (const ShaderInputDescriptorT& shaderInput : shaderInputDescriptors)
			{
				++currentGroupSize;
				++shaderInputIndex;
			}
			groupSize = currentGroupSize;
			return true;
		}

		Interval ShaderResourceGroupLayout::GetGroupInterval(ShaderInputBufferIndex inputIndex) const
		{
			return m_intervalsForBuffers[inputIndex.GetIndex()];
		}

		Interval ShaderResourceGroupLayout::GetGroupInterval(ShaderInputImageIndex inputIndex) const
		{
			return m_intervalsForImages[inputIndex.GetIndex()];
		}

		Interval ShaderResourceGroupLayout::GetGroupInterval(ShaderInputSamplerIndex inputIndex) const
		{
			return m_intervalsForSamplers[inputIndex.GetIndex()];
		}

		uint32_t ShaderResourceGroupLayout::GetGroupSizeForBuffers() const
		{
			return m_groupSizeForBuffers;
		}

		uint32_t ShaderResourceGroupLayout::GetGroupSizeForImages() const
		{
			return m_groupSizeForImages;
		}

		uint32_t ShaderResourceGroupLayout::GetGroupSizeForBufferUnboundedArrays() const
		{
			return m_groupSizeForBufferUnboundedArrays;
		}

		uint32_t ShaderResourceGroupLayout::GetGroupSizeForImageUnboundedArrays() const
		{
			return m_groupSizeForImageUnboundedArrays;
		}

		uint32_t ShaderResourceGroupLayout::GetGroupSizeForSamplers() const
		{
			return m_groupSizeForSamplers;
		}

		ShaderInputBufferIndex ShaderResourceGroupLayout::FindShaderInputBufferIndex(const std::string& name) const
		{
			return m_nameToIdxReflectionForBuffers.find(name)->second;
		}

		ShaderInputImageIndex ShaderResourceGroupLayout::FindShaderInputImageIndex(const std::string& name) const
		{
			return m_nameToIdxReflectionForImages.find(name)->second;
		}
	}
}