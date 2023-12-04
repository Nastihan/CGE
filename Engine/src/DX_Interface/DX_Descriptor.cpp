#include "DX_Descriptor.h"

namespace CGE
{
	namespace DX12
	{
		DX_DescriptorHandle::DX_DescriptorHandle() : m_index{ NullIndex }, m_type{ D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES }, m_flags{ D3D12_DESCRIPTOR_HEAP_FLAG_NONE } {}
		DX_DescriptorHandle::DX_DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t index) : m_index{ index }, m_type{ type }, m_flags{ flags } {}
		DX_DescriptorHandle DX_DescriptorHandle::operator+(uint32_t offset) const
		{
			return DX_DescriptorHandle(m_type, m_flags, m_index + offset);
		}
		DX_DescriptorHandle& DX_DescriptorHandle::operator+=(uint32_t offset)
		{
			m_index += offset;
			return *this;
		}
		bool DX_DescriptorHandle::IsNull() const
		{
			return m_type == D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		}
		bool DX_DescriptorHandle::IsShaderVisible() const
		{
			return m_flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		}

        DX_DescriptorTable::DX_DescriptorTable(DX_DescriptorHandle handle, uint32_t count) : m_offset(handle) , m_size(count) {}

        DX_DescriptorHandle DX_DescriptorTable::operator[](uint32_t i) const
        {
            return m_offset + i;
        }

        DX_DescriptorHandle DX_DescriptorTable::GetOffset() const
        {
            return m_offset;
        }

        D3D12_DESCRIPTOR_HEAP_TYPE DX_DescriptorTable::GetType() const
        {
            return m_offset.m_type;
        }

        D3D12_DESCRIPTOR_HEAP_FLAGS DX_DescriptorTable::GetFlags() const
        {
            return m_offset.m_flags;
        }

        uint32_t DX_DescriptorTable::GetSize() const
        {
            return m_size;
        }

        bool DX_DescriptorTable::IsNull() const
        {
            return m_offset.IsNull() || !m_size;
        }

        bool DX_DescriptorTable::IsValid() const
        {
            return !IsNull();
        }
	}
}