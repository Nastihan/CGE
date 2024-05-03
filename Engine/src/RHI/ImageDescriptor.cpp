
// RHI
#include "ImageDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		Size::Size(uint32_t width, uint32_t height, uint32_t depth) : m_width{ width }, m_height{ height }, m_depth{ depth } {}

		Size Size::GetReducedMip(uint32_t mipLevel) const
		{
			// Each mip level will be half of the dimensions of the previous mip
			// We shift to divide by two
			Size size;
			size.m_width = std::max(m_width >> mipLevel, 1u);
			size.m_height = std::max(m_height >> mipLevel, 1u);
			size.m_depth = std::max(m_depth >> mipLevel, 1u);
			return size;
		}

		bool Size::operator==(const Size& rhs) const
		{
			return m_width == rhs.m_width && m_height == rhs.m_height && m_depth == rhs.m_depth;
		}

		bool Size::operator!=(const Size& rhs) const
		{
			return m_width != rhs.m_width || m_height != rhs.m_height || m_depth != rhs.m_depth;
		}

		uint32_t& Size::operator[](uint32_t idx)
		{
			uint32_t* ptr = &m_width;
			return *(ptr + idx);
		}

		uint32_t Size::operator[](uint32_t idx) const
		{
			const uint32_t* ptr = &m_width;
			return *(ptr + idx);
		}

		// ==============================================================================================

		ImageDescriptor ImageDescriptor::Create1D(ImageBindFlags bindFlags, uint32_t width, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_dimension = ImageDimension::Image1D;
			descriptor.m_size.m_width = width;
			descriptor.m_format = format;
			return descriptor;
		}

		ImageDescriptor ImageDescriptor::Create1DArray(ImageBindFlags bindFlags, uint32_t width, uint16_t arraySize, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_dimension = ImageDimension::Image1D;
			descriptor.m_size.m_width = width;
			descriptor.m_arraySize = arraySize;
			descriptor.m_format = format;
			return descriptor;
		}

		ImageDescriptor ImageDescriptor::Create2D(ImageBindFlags bindFlags, uint32_t width, uint32_t height, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_size.m_width = width;
			descriptor.m_size.m_height = height;
			descriptor.m_format = format;
			return descriptor;
		}

		ImageDescriptor ImageDescriptor::Create2DArray(ImageBindFlags bindFlags, uint32_t width, uint32_t height, uint16_t arraySize, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_size.m_width = width;
			descriptor.m_size.m_height = height;
			descriptor.m_arraySize = arraySize;
			descriptor.m_format = format;
			return descriptor;
		}

		ImageDescriptor ImageDescriptor::CreateCubemap(ImageBindFlags bindFlags, uint32_t width, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_size.m_width = width;
			descriptor.m_size.m_height = width;
			descriptor.m_arraySize = NumCubeMapSlices;
			descriptor.m_format = format;
			descriptor.m_isCubemap = true;
			return descriptor;
		}

		ImageDescriptor ImageDescriptor::CreateCubemapArray(ImageBindFlags bindFlags, uint32_t width, uint16_t arraySize, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_size.m_width = width;
			descriptor.m_size.m_height = width;
			descriptor.m_arraySize = NumCubeMapSlices * arraySize;
			descriptor.m_format = format;
			descriptor.m_isCubemap = true;
			return descriptor;
		}

		ImageDescriptor ImageDescriptor::Create3D(ImageBindFlags bindFlags, uint32_t width, uint32_t height, uint32_t depth, Format format)
		{
			ImageDescriptor descriptor;
			descriptor.m_bindFlags = bindFlags;
			descriptor.m_dimension = ImageDimension::Image3D;
			descriptor.m_size.m_width = width;
			descriptor.m_size.m_height = height;
			descriptor.m_size.m_depth = depth;
			descriptor.m_format = format;
			return descriptor;
		}

		HashValue64 ImageDescriptor::GetHash(HashValue64 seed) const
		{
			return TypeHash64(*this, seed);
		}
	}
}