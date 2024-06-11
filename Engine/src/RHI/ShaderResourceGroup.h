#pragma once

// RHI
#include "Resource.h"
#include "ShaderResourceGroupData.h"

namespace CGE
{
	namespace RHI
	{
		// Shader resource group will be a bundle of frequency based shader resource views that get bound to the pipeline for GPU commands.
		// These resource groups will become platform dependant tables, samplers and constants which will be set on the pipeline layout for shader use.
		// All operations will get queued and happen on a pool.
		// As an example these shader resource groups will get root descriptor bindings in the dx12 backend.
		// (Check Atom deep dive for more info: https://youtu.be/brLHCsmfr-I?si=I3XVUDGuQieOkRzh&t=1437)
		class ShaderResourceGroup : public Resource
		{
		public:
			virtual ~ShaderResourceGroup() override = default;

			// Initilizes the tables based on the group layout.
			ResultCode Init(Device& device, const ShaderResourceGroupData& shaderResourceGroupData);

			// Must be called after init.
			// This function will queue a compile (updating invalid views and tables) on the pool (compilation will be deferred).
			// The function will set the view data passed in and copy all the descriptors into continuous locations in the shader visible heap for the descriptor tables.
			// [todo] For now I will compile once and but later have to change so if a view gets invalidated a recompile is needed.
			// Also if the user adds views to the srg we need another recompile.
			ResultCode Compile();
			const ShaderResourceGroupData& GetData() const;
			const ShaderResourceGroupLayout* GetLayout() const;
			uint32_t GetBindingSlot() const;

			// Update the m_rhiUpdateMask for a given resource type which will ensure we will compile that type for the current frame
			void EnableRhiResourceTypeCompilation(const ShaderResourceGroupData::ResourceTypeMask resourceTypeMask);

			HashValue64 GetViewHash(const std::string& viewName);
			void UpdateViewHash(const std::string& viewName, const HashValue64 viewHash);

		protected:
			ShaderResourceGroup() = default;

			virtual ResultCode InitInternal(const ShaderResourceGroupData& shaderResourceGroupData) = 0;
			virtual ResultCode CompileInternal() = 0;

			// Call every frame to check for recompilation of the views into tables.
			// This function will modify the m_rhiUpdateMask if it finds any changes in the hash values of the set ShaderResourceGroupData and the last cached values in m_viewHash.
			void ResetUpdateMaskForModifiedViews();
			bool IsAnyResourceTypeUpdated() const;
			bool IsResourceTypeEnabledForCompilation(uint32_t resourceTypeMask) const;

		private:
			void SetData(const ShaderResourceGroupData& data);
			
			// Calculate the hash for all the views passed in
			template<typename T>
			HashValue64 GetViewHash(std::span<const RHI::ConstPtr<T>> views);
			// This will update the resource mask based on the type and view span passed in.
			template<typename T>
			void UpdateMaskBasedOnViewHash(const std::string& shaderInputDescriptorName, std::span<const RHI::ConstPtr<T>> views, ShaderResourceGroupData::ResourceType resourceType);

		private:
			// This will house all the RHI resource views the srg will compile.
			ShaderResourceGroupData m_shaderResourceGroupData;

			// Logical binding slot from high to low frequency on change.
			uint32_t m_bindingSlot = static_cast<uint32_t>(-1);

			// This mask will be used to check which group of resources (Image, buffer and sampler) needs to be recompiled.
			// Once a view gets invalidated for example.
			uint32_t m_rhiUpdateMask = 0;
			bool m_needCompile = true;

			// The name will correspond to the name of the ShaderInputDescriptor. (Exists in the ShaderResourceGroupData::m_groupLaout Like ShaderInputImageDescriptor::m_name)
			std::unordered_map<std::string, HashValue64> m_viewHash;
		};
	}
}