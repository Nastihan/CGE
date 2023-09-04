#pragma once
#include <Windows.h>
#include <stdexcept>

namespace CGE
{
	class DX_Exception : public std::exception
	{
	public:
		DX_Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept;
	private:
		int GetLine() const noexcept;
		const std::string& GetFile() const noexcept;
		std::string GetOriginString() const noexcept;
		std::string TranslateErrorCode(HRESULT hr) const noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string SwitchD3d12ErrorCode() const noexcept;
	private:
		HRESULT hr;
		int line;
		std::string file;
		mutable std::string whatBuffer;
	};
}