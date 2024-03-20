#pragma once

// RHI
#include "../RHI/ResourcePool.h"

namespace CGE
{
	namespace DX12
	{
        class DX_ResourcePoolResolver : public RHI::ResourcePoolResolver
        {
        public:
            virtual ~DX_ResourcePoolResolver() = default;

            // Called when a resource from the pool is being Shutdown
            virtual void OnResourceShutdown(const RHI::Resource& resource) {}
        };
	}
}