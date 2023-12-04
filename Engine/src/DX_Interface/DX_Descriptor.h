#pragma once

#include "DX_CommonHeaders.h"

namespace CGE
{
	namespace DX12
	{
		struct DX_DescriptorHandle
		{
			DX_DescriptorHandle();
			DX_DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t index);

			DX_DescriptorHandle operator+(uint32_t offset) const;
			DX_DescriptorHandle& operator+=(uint32_t offset);
			
			bool IsNull() const;
			bool IsShaderVisible() const;

			static const uint32_t NullIndex = (uint32_t)-1;

			// This syntax is bit-fields (number of bits each member should occupy)
			uint32_t m_index : 32;
			D3D12_DESCRIPTOR_HEAP_TYPE m_type : 4;
			D3D12_DESCRIPTOR_HEAP_FLAGS m_flags : 4;
		};

		class DX_DescriptorTable
		{
		public:
			DX_DescriptorTable() = default;
			DX_DescriptorTable(DX_DescriptorHandle handle, uint32_t count);

			DX_DescriptorHandle operator[](uint32_t i) const;

			DX_DescriptorHandle GetOffset() const;
			D3D12_DESCRIPTOR_HEAP_TYPE GetType() const;
			D3D12_DESCRIPTOR_HEAP_FLAGS GetFlags() const;
			uint32_t GetSize() const;

			bool IsNull() const;
			bool IsValid() const;

		private:
			DX_DescriptorHandle m_offset;
			uint32_t m_size = 0;
		};
	}
}