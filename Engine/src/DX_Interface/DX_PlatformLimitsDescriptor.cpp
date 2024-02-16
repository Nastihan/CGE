
// DX12
#include "DX_PlatformLimitsDescriptor.h"

namespace CGE
{
	namespace DX12
	{
		void DX_PlatformLimitsDescriptor::LoadPlatformLimitsDescriptor(const char* rhiName)
		{
			m_descriptorHeapLimits = std::unordered_map<std::string, std::array<uint32_t, NumHeapFlags>>({
						{ std::string("DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV"), { 100'000, 1'000'000 } },
						{ std::string("DESCRIPTOR_HEAP_TYPE_SAMPLER"), { 2048, 2048 } },
						{ std::string("DESCRIPTOR_HEAP_TYPE_RTV"), { 2048, 0 } },
						{ std::string("DESCRIPTOR_HEAP_TYPE_DSV"), { 2048, 0 } }
				});
		}
	}
}