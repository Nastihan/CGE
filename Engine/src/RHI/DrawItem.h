#pragma once

#include "StreamBufferView.h"
#include "IndexBufferView.h"

namespace CGE
{
	namespace RHI
	{
		struct DrawItem
		{
			uint8_t m_streamBufferViewCount = 0;

			// Vertex buffer view
			const StreamBufferView* m_streamBufferViews = nullptr;
			
			// Index buffer view
			const IndexBufferView* m_indexBufferView = nullptr;
		};
	}
}