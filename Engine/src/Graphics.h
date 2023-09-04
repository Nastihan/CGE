#pragma once
#include "Backend_API_Headers.h"

namespace CGE
{
	//[todo] This class will be our high level renderer
	class Graphics
	{
	public:
		Graphics();
		~Graphics();
		Graphics(const Graphics&) = delete;
		Graphics(Graphics&&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		Graphics& operator=(Graphics&&) = delete;
	public:
		void Render();
	private:
		// [todo] garbage interface
		const wrl::ComPtr<ID3D12Device8>& device = DX_Device::GetInstance().GetDevice();
		DX_Commander commander;
	};
}