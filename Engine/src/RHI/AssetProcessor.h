#pragma once

// RHI
#include "RHI_Common.h"
#include "Device.h"
#include "PipelineState.h"
#include "ShaderCompiler.h"

#include "../CommonMacros.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace CGE
{
	namespace RHI
	{
		enum class ShaderPermutationType : uint32_t
		{
			Engine = 0,
			Material,
			Unknown
		};

		struct ShaderPermutation
		{
			ShaderPermutation() = default;

			RHI::HardwareQueueClass m_queueClass = RHI::HardwareQueueClass::Unknown;
			ShaderPermutationType m_type = RHI::ShaderPermutationType::Unknown;

			RHI::RenderStates m_renderState;
			RHI::InputStreamLayout m_inputStreamLayout;
			RHI::Ptr<ShaderStageFunction> m_vertexShader;
			RHI::Ptr<ShaderStageFunction> m_pixelShader;

			RHI::Ptr<ShaderStageFunction> m_computeShader;
			
			RHI::Ptr<PipelineLayoutDescriptor> m_pipelineLayoutDescriptor;
		};

		class AssetProcessor
		{
		public:
			REMOVE_COPY_AND_MOVE(AssetProcessor);
			AssetProcessor();
			~AssetProcessor() = default;

			void InitShaderCompiler();
			RHI::ResultCode BuildShaderPermutations();
			const std::shared_ptr<const ShaderPermutation> GetShaderPermutation(std::string name) const;

		private:
			void SetRasterState(const nlohmann::json& jRasterState, RHI::RasterState& rasterState);
			void SetDepthStencilState(const nlohmann::json& jDepthStencilState, RHI::DepthStencilState& depthStencilState);
			void SetBlendState(nlohmann::json jBlendState, RHI::TargetBlendState& blendState);
			void CompileAndSetShaderStates(nlohmann::json jProgramSettings);

			// String to RHI type conversions
			RHI::FillMode GetFillMode(const std::string& str);
			RHI::CullMode GetCullMode(const std::string& str);
			RHI::DepthWriteMask GetDepthWriteMask(const std::string& str);
			RHI::ComparisonFunc GetComparisonFunc(const std::string& str);
			RHI::StencilOp GetStencilOp(const std::string& str);
			RHI::BlendFactor GetBlendFactor(const std::string& str);
			RHI::BlendOp GetBlendOp(const std::string& str);
			RHI::ShaderStage GetShaderStage(const std::string& str);
			RHI::ShaderPermutationType GetShaderPermutationType(const std::string& str);
			RHI::HardwareQueueClass GetHardwareQueueClass(const std::string& str);

		private:
			std::string m_fullAssetPath;
			std::string m_fullShaderAssetPath;
			std::string m_fullModelAssetPath;

			std::vector<std::string> m_shaderFiles;

			// [todo] Hash to lookup duplicates
			std::unordered_map<std::string, std::shared_ptr<ShaderPermutation>> m_permutationMap;

			RHI::Ptr<RHI::ShaderCompiler> m_shaderCompiler;
		};
	}
}