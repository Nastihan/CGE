
// DX12
#include "DX_PipelineLayout.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_PipelineLayout> DX_PipelineLayout::Create()
		{
			return new DX_PipelineLayout();
		}
	}
}