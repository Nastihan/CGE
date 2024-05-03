#pragma once

// RHI
#include "ResourceView.h"
#include "TypeHash.h"
#include "ImageViewDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        class Image;

        class ImageView : public ResourceView
        {
        public:
            virtual ~ImageView() = default;

            ResultCode Init(const Image& image, const ImageViewDescriptor& viewDescriptor);
            const ImageViewDescriptor& GetDescriptor() const;
            const Image& GetImage() const;

            // The view cover all the imagesubresources.
            bool IsFullView() const override final;
            HashValue64 GetHash() const;

            static constexpr uint32_t InvalidBindlessIndex = 0xFFFFFFFF;
            virtual uint32_t GetBindlessReadIndex() const { return InvalidBindlessIndex; }
            virtual uint32_t GetBindlessReadWriteIndex() const { return InvalidBindlessIndex; }

        protected:
            HashValue64 m_hash = HashValue64{ 0 };

        private:
            bool ValidateForInit(const Image& image, const ImageViewDescriptor& viewDescriptor) const;

        private:
            ImageViewDescriptor m_descriptor;
        };
	}
}