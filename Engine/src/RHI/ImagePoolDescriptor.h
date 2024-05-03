#pragma once

// RHI
#include "ResourcePoolDescriptor.h"
#include "ImageEnums.h"

namespace CGE
{
	namespace RHI
	{
        class ImagePoolDescriptor : public ResourcePoolDescriptor
        {
        public:
            virtual ~ImagePoolDescriptor() = default;
            ImagePoolDescriptor() = default;

            ImageBindFlags m_bindFlags = ImageBindFlags::Color;
        };
	}
}