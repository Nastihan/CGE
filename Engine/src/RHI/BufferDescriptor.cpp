
// RHI
#include "BufferDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		HashValue64 BufferDescriptor::GetHash(HashValue64 seed) const
		{
			return TypeHash64(*this, seed);
		}
	}
}