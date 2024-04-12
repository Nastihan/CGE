
// DX12
#include "DX_ImagePoolResolver.h"

namespace CGE
{
	namespace DX12
	{
		DX_ImagePoolResolver::DX_ImagePoolResolver(DX_Device& device, DX_ImagePool* imagePool)
		{

		}

		RHI::ResultCode DX_ImagePoolResolver::UpdateImage(const RHI::ImageUpdateRequest& request, size_t& bytesTransferred)
		{
			return RHI::ResultCode::Fail;
		}

		void DX_ImagePoolResolver::OnResourceShutdown(const RHI::Resource& resource)
		{

		}
	}
}