
// RHI
#include "ImageSystem.h"
#include "Graphics.h"
#include "Image.h"
#include "ImagePoolDescriptor.h"
#include "ImagePool.h"
#include "ImageViewDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		void ImageSystem::Init(RHI::Device& device)
		{
			DeviceObject::Init(device);
            CreateShaderReadImagePool();
			m_initialized = true;
		}

		void ImageSystem::Shutdown()
		{
			if (!m_initialized)
			{
				return;
			}
            m_shaderReadImagePool = nullptr;
			m_initialized = false;
		}

		RHI::Ptr<RHI::ImagePool> ImageSystem::GetSimpleImagePool()
		{
			return m_shaderReadImagePool;
		}

		bool ImageSystem::CreateShaderReadImagePool()
		{
            if (!m_initialized)
            {
                return false;
            }
            auto* device = &GetDevice();

            RHI::Ptr<RHI::ImagePool> imagePool = Graphics::GetFactory().CreateImagePool();
            RHI::ImagePoolDescriptor imagePoolDesc;
            imagePoolDesc.m_bindFlags = RHI::ImageBindFlags::ShaderRead;
            imagePool->SetName("ShaderReadImagePool");
            RHI::ResultCode resultCode = imagePool->Init(*device, imagePoolDesc);
            if (resultCode != RHI::ResultCode::Success)
            {
                assert(false, "Failed to create image pool");
                return false;
            }
            m_shaderReadImagePool = imagePool;
            return true;
		}
	}
}