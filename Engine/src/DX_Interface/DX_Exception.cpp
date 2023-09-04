#include "DX_Exception.h"
#include <sstream>

namespace CGE
{
    DX_Exception::DX_Exception(int line, const char* file, HRESULT hr) noexcept : line{ line }, file{ file }, hr{ hr } {}

    const char* DX_Exception::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl 
            << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() 
            << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl << std::endl
            << "[Description] " << TranslateErrorCode(hr) << std::endl 
            << GetOriginString();
        whatBuffer = oss.str();
        return whatBuffer.c_str();
    }
    const char* DX_Exception::GetType() const noexcept
    {
        return "DX12 Exception";
    }
    int DX_Exception::GetLine() const noexcept
    {
        return line;
    }
    const std::string& DX_Exception::GetFile() const noexcept
    {
        return file;
    }
    std::string DX_Exception::GetOriginString() const noexcept
    {
        std::ostringstream oss;
        oss << "[File] " << file << std::endl << "[Line] " << line;
        return oss.str();
    }
    std::string DX_Exception::TranslateErrorCode(HRESULT hr) const noexcept
    {
        char* pMsgBuf = nullptr;
        // windows will allocate memory for err string and make our pointer point to it
        const DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);
        // 0 string length returned indicates a failure
        if (nMsgLen == 0)
        {
            return SwitchD3d12ErrorCode();
        }
        // copy error string from windows-allocated buffer to std::string
        std::string errorString = pMsgBuf;
        // free windows buffer
        LocalFree(pMsgBuf);
        return errorString;
    }
    HRESULT DX_Exception::GetErrorCode() const noexcept
    {
        return hr;
    }
    std::string DX_Exception::SwitchD3d12ErrorCode() const noexcept
    {
        DWORD errorCode = HRESULT_CODE(hr);
        switch (errorCode) {
        case D3D12_ERROR_ADAPTER_NOT_FOUND:
            return "Direct3D 12 Adapter Not Found";
        case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
            return "Direct3D 12 Driver Version Mismatch";
        case DXGI_ERROR_INVALID_CALL:
            return "DXGI Invalid Call";
        case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
            return "DXGI Not Currently Available";
        case DXGI_ERROR_UNSUPPORTED:
            return "DirectX Unsupported";
        case DXGI_ERROR_DEVICE_HUNG:
            return "Direct3D 12 Device Hung";
        case DXGI_ERROR_DEVICE_REMOVED:
            return "Direct3D 12 Device Removed";
        case DXGI_ERROR_DEVICE_RESET:
            return "Direct3D 12 Device Reset";
        case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
            return "DXGI Driver Internal Error";
        case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
            return "DXGI Frame Statistics Disjoint";
        case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return "DXGI Graphics VidPN Source In Use";
        case DXGI_ERROR_MORE_DATA:
            return "DXGI More Data";
        case DXGI_ERROR_NONEXCLUSIVE:
            return "DXGI Nonexclusive";
        case DXGI_ERROR_NOT_FOUND:
            return "DXGI Not Found";
        case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:
            return "DXGI Remote Client Disconnected";
        case DXGI_ERROR_REMOTE_OUTOFMEMORY:
            return "DXGI Remote Out of Memory";
        case DXGI_ERROR_WAS_STILL_DRAWING:
            return "DXGI Was Still Drawing";
        case DXGI_ERROR_ACCESS_LOST:
            return "DXGI Access Lost";
        case DXGI_ERROR_WAIT_TIMEOUT:
            return "DXGI Wait Timeout";
        case DXGI_ERROR_SESSION_DISCONNECTED:
            return "DXGI Session Disconnected";
        case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
            return "DXGI Restrict To Output Stale";
        case DXGI_ERROR_CANNOT_PROTECT_CONTENT:
            return "DXGI Cannot Protect Content";
        case DXGI_ERROR_ACCESS_DENIED:
            return "DXGI Access Denied";
        case DXGI_ERROR_NAME_ALREADY_EXISTS:
            return "DXGI Name Already Exists";
        case DXGI_ERROR_SDK_COMPONENT_MISSING:
            return "DXGI SDK Component Missing";
        case DXGI_ERROR_NOT_CURRENT:
            return "DXGI Not Current";
        case DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY:
            return "DXGI HW Protection Out of Memory";
        case DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION:
            return "DXGI Dynamic Code Policy Violation";
        case DXGI_ERROR_NON_COMPOSITED_UI:
            return "DXGI Non Composited UI";
        case DXGI_STATUS_OCCLUDED:
            return "DXGI Status Occluded";
        case DXGI_STATUS_CLIPPED:
            return "DXGI Status Clipped";
        case DXGI_STATUS_NO_REDIRECTION:
            return "DXGI Status No Redirection";
        case DXGI_STATUS_NO_DESKTOP_ACCESS:
            return "DXGI Status No Desktop Access";
        case DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return "DXGI Graphics VidPN Source In Use";
        case DXGI_STATUS_MODE_CHANGED:
            return "DXGI Status Mode Changed";
        case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS:
            return "DXGI Status Mode Change In Progress";
        default:
            return "Unknown DirectX Error, please check output log";
        }
    }
}