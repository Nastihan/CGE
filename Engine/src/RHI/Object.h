#pragma once
#include "intrusive_ptr.h"
#include <string>
#include <atomic>

namespace CGE
{
	namespace RHI
	{
		class Object
		{
		public:
			virtual ~Object() = default;
			void SetName(const std::string& name);
			const std::string& GetName() const;
			uint32_t use_count();
		protected:
			Object() = default;

			template<typename Type>
			friend struct IntrusivePtrCountPolicy;
			void add_ref() const;
			void release() const;
		private:
			virtual void Shutdown() {};
			virtual void SetNameInternal(const std::string& name);

		protected:
			mutable std::atomic_int m_useCount = 0;
		private:
			std::string m_name;
		};
	}
}