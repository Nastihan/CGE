#include "CGE_Exception.h"
#include <sstream>

namespace CGE
{
	CGE_Exception::CGE_Exception(int line, const char* file) noexcept : line(line), file(file) {}

	const char* CGE_Exception::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl << GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* CGE_Exception::GetType() const noexcept
	{
		return "CGE_Exception";
	}

	int CGE_Exception::GetLine() const noexcept
	{
		return line;
	}

	const std::string& CGE_Exception::GetFile() const noexcept
	{
		return file;
	}

	std::string CGE_Exception::GetOriginString() const noexcept
	{
		std::ostringstream oss;
		oss << "[File] " << file << std::endl << "[Line] " << line;
		return oss.str();
	}
}