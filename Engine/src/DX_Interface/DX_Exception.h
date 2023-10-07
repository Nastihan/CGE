#pragma once
#include "../CGE_Exception.h"
#include <Windows.h>
#include <stdexcept>

namespace CGE
{
	namespace DX12
	{
		class DX_Exception : public CGE_Exception
		{
		public:
			DX_Exception(int line, const char* file, HRESULT hr) noexcept;
			const char* what() const noexcept override;
			const char* GetType() const noexcept;
		protected:
			int GetLine() const noexcept;
			const std::string& GetFile() const noexcept;
			std::string GetOriginString() const noexcept;
			HRESULT GetErrorCode() const noexcept;
			std::string SwitchD3d12ErrorCode() const noexcept;
		protected:
			HRESULT hr;
			int line;
			std::string file;
			mutable std::string whatBuffer;
		};
		class DX_DeviceRemovedException : public DX_Exception
		{
			using DX_Exception::DX_Exception;
		public:
			const char* what() const noexcept override;
			const char* GetType() const noexcept override;
		private:
			std::string reason;
		};
	}
}