
// RHI
#include "PipelineState.h"

namespace CGE
{
	namespace RHI
	{
        ResultCode PipelineState::Init(Device& device, const PipelineStateDescriptorForDraw& descriptor, PipelineLibrary* pipelineLibrary)
        {
            if (IsInitialized())
            {
                return ResultCode::InvalidOperation;
            }

            bool error = false;

            if (!descriptor.m_inputStreamLayout.IsFinalized())
            {
                assert(false, "InputStreamLayout is not finalized!");
                error = true;
            }

            if (error)
            {
                return ResultCode::InvalidOperation;
            }

            const ResultCode resultCode = InitInternal(device, descriptor, pipelineLibrary);

            if (resultCode == ResultCode::Success)
            {
                m_type = PipelineStateType::Draw;
                DeviceObject::Init(device);
            }

            return resultCode;
        }

        PipelineStateType PipelineState::GetType() const
        {
            return m_type;
        }

        void PipelineState::Shutdown()
        {
            if (IsInitialized())
            {
                ShutdownInternal();
                DeviceObject::Shutdown();
            }
        }
	}
}