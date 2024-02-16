
// DX12
#include "DX_BufferPoolDescriptor.h"

namespace CGE
{
    namespace DX12
    {
        DX_BufferPoolDescriptor::DX_BufferPoolDescriptor(const RHI::Device& device)
        {
            m_bufferPoolPageSizeInBytes = device.GetPlatformLimitsDescriptor().m_platformDefaultValues.m_bufferPoolPageSizeInBytes;
        }
    }
}