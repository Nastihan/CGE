#pragma once

// RHI
#include "../RHI/ResourcePool.h"

namespace CGE
{
	namespace DX12
	{
        class DX_CommandList;
        class DX_Scope;

        class DX_ResourcePoolResolver : public RHI::ResourcePoolResolver
        {
        public:
            virtual ~DX_ResourcePoolResolver() = default;

            // Called during compilation of the frame, prior to execution.
            virtual void Compile(DX_Scope& scope) {}

            // Queues transition barriers at the beginning of a scope.
            virtual void QueuePrologueTransitionBarriers(DX_CommandList&) {}

            // Performs resolve-specific copy / streaming operations. (Call at the begining of the frame(root scope))
            virtual void Resolve(DX_CommandList&) const {}

            // Queues transition barriers at the end of a scope.
            virtual void QueueEpilogueTransitionBarriers(DX_CommandList&) const {}

            // Called at the end of the frame after execution.
            virtual void Deactivate() {}

            // Called when a resource from the pool is being Shutdown
            virtual void OnResourceShutdown(const RHI::Resource& resource) {}
        };
	}
}