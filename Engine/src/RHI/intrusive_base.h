#pragma once

#include "intrusive_refcount.h"

namespace CGE
{
	namespace RHI
	{
		using intrusive_base = intrusive_refcount<std::atomic_uint>;
	}
}