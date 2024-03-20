#pragma once

// RHI
#include "DeviceObject.h"
#include "PipelineStateDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		class PipelineLibrary;
		class PipelineState : public DeviceObject
		{
		public:
			virtual ~PipelineState() = default;

			// PipelineLibrary will be null for now. Used later for caching the pso.
			ResultCode Init(Device& device, const PipelineStateDescriptorForDraw& descriptor, PipelineLibrary* pipelineLibrary = nullptr);
			PipelineStateType GetType() const;

		private:
			void Shutdown() override final;

			virtual ResultCode InitInternal(Device& device, const PipelineStateDescriptorForDraw& descriptor, PipelineLibrary* pipelineLibrary) = 0;
			virtual void ShutdownInternal() = 0;

			PipelineStateType m_type = PipelineStateType::Count;
		};
	}
}