#pragma once

// RHI
#include "RHI_Common.h"
#include "DeviceObject.h"
#include "Format.h"
#include "ImagePool.h"

namespace CGE
{
	namespace RHI
	{
        class Image;

		class ImageSystem : public DeviceObject
		{
        public:
            ImageSystem() = default;

            void Init(RHI::Device& device);
            void Shutdown();
            RHI::Ptr<RHI::ImagePool> GetSimpleImagePool();

        protected:
            bool CreateImagePool();

        private:
            // For now keeping it simple pool for textures used in shaders.
            // [todo] Initialize different pools for different type of attachments and resources.
            RHI::Ptr<RHI::ImagePool> m_imagePool;
            bool m_initialized = false;
		};
	}
}