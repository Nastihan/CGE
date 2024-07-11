#include "DX_Factory.h"

#include "DX_PhysicalDevice.h"
#include "DX_Device.h"
#include "DX_SwapChain.h"
#include "DX_FrameGraphExecuter.h"
#include "DX_BufferView.h"
#include "DX_Buffer.h"
#include "DX_BufferPool.h"
#include "DX_ImageView.h"
#include "DX_Image.h"
#include "DX_ImagePool.h"
#include "DX_PipelineLayoutDescriptor.h"
#include "DX_PipelineLayout.h"
#include "DX_ShaderStageFunction.h"
#include "DX_PipelineState.h"
#include "DX_ShaderResourceGroup.h"
#include "DX_ShaderCompiler.h"

namespace CGE
{
	namespace DX12
	{
		DX_Factory::DX_Factory()
		{
			m_backendName = "DX12";
		}

		RHI::Ptr<RHI::PhysicalDevice> DX_Factory::CreatePhysicalDevice()
		{
			return DX_PhysicalDevice::Create();
		}

		RHI::Ptr<RHI::Device> DX_Factory::CreateDevice()
		{
			return DX_Device::Create();
		}

		RHI::Ptr<RHI::SwapChain> DX_Factory::CreateSwapChain()
		{
			return DX_SwapChain::Create();
		}

		RHI::Ptr<RHI::FrameGraphExecuter> DX_Factory::CreateFrameGraphExecuter()
		{
			return DX_FrameGraphExecuter::Create();
		}

		RHI::Ptr<RHI::BufferView> DX_Factory::CreateBufferView()
		{
			return DX_BufferView::Create();
		}

		RHI::Ptr<RHI::Buffer> DX_Factory::CreateBuffer()
		{
			return DX_Buffer::Create();
		}

		RHI::Ptr<RHI::BufferPool> DX_Factory::CreateBufferPool()
		{
			return DX_BufferPool::Create();
		}

		RHI::Ptr<RHI::ImageView> DX_Factory::CreateImageView()
		{
			return DX_ImageView::Create();
		}

		RHI::Ptr<RHI::Image> DX_Factory::CreateImage()
		{
			return DX_Image::Create();
		}

		RHI::Ptr<RHI::ImagePool> DX_Factory::CreateImagePool()
		{
			return DX_ImagePool::Create();
		}

		RHI::Ptr<RHI::PipelineState> DX_Factory::CreatePipelineState()
		{
			return DX_PipelineState::Create();
		}

		RHI::Ptr<RHI::PipelineLayoutDescriptor> DX_Factory::CreatePipelineLayoutDescriptor()
		{
			return DX_PipelineLayoutDescriptor::Create();
		}

		RHI::Ptr<RHI::ShaderStageFunction> DX_Factory::CreateShaderStageFunction()
		{
			return DX_ShaderStageFunction::Create();
		}

		RHI::Ptr<RHI::ShaderResourceGroup> DX_Factory::CreateShaderResourceGroup()
		{
			return DX_ShaderResourceGroup::Create();
		}

		RHI::Ptr<RHI::ShaderCompiler> DX_Factory::CreateShaderCompiler()
		{
			return DX_ShaderCompiler::Create();
		}
	}
}