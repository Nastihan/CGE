#include "Object.h"

#include <assert.h>

namespace CGE
{
	namespace RHI
	{
		void Object::SetName(const std::string& name)
		{
			m_name = name;
		}
		const std::string& Object::GetName() const
		{
			return m_name;
		}
		uint32_t Object::use_count()
		{
			return static_cast<uint32_t>(m_useCount);
		}
		void Object::add_ref() const
		{
			assert(m_useCount >= 0, "m_useCount is negative");
			m_useCount++;
		}
		void Object::release() const
		{
			m_useCount--;
			assert(m_useCount >= 0, "Releasing an already released object");

			int expectedRefCount = 0;
			if (m_useCount.compare_exchange_strong(expectedRefCount, -1))
			{
				Object* object = const_cast<Object*>(this);
				object->Shutdown();
				delete object;
			}
		}
		void Object::SetNameInternal(const std::string& name) {}
	}
}