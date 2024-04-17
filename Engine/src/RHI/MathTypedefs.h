#pragma once

#include <DirectXMath.h>

namespace CGE
{
	namespace RHI
	{
		namespace Math
		{
#ifdef USE_DX12
			typedef DirectX::XMFLOAT4X4 Matrix44;
			typedef DirectX::XMMATRIX SIMDMatrix44;
			typedef DirectX::XMFLOAT4 Vector4;
			typedef DirectX::XMFLOAT3 Vector3;
			typedef DirectX::XMFLOAT2 Vector2;
#endif // !USE_DX12

#ifdef USE_VULKAN

#endif // !USE_VULKAN
		}
	}
}