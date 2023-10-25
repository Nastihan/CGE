#include "DeviceObject.h"

namespace CGE
{
	namespace RHI
	{

		bool DeviceObject::IsInitilized() const
		{
			return m_device != nullptr;
		}
		Device& DeviceObject::GetDevice() const
		{
			return *m_device;
		}
		void DeviceObject::Init(Device& device)
		{
			m_device = &device;
		}
		void DeviceObject::Shutdown()
		{
			m_device = nullptr;
		}
	}
}