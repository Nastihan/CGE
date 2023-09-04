#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include <stdexcept>
#include <cassert>
#include "DX_Exception.h"
#include "../CommonMacros.h"
#include "../ConsoleLog.h"

namespace CGE
{
#ifndef LOCAL_HR
#define LOCAL_HR HRESULT hr;
#endif // !LOCAL_HR

#ifndef DX_THROW_FAILED
#define DX_THROW_FAILED(hrCall) if(FAILED(hr = hrCall)) throw DX_Exception(__LINE__, __FILE__, hr);
#endif // !DX_EXEPTION


    namespace wrl = Microsoft::WRL;

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }

    template<typename T>
    constexpr void Release(T*& resource)
    {
        if (resource)
        {
            resource->Release();
            resource = nullptr;
        }
    }

#ifdef _DEBUG
// sets the name of the d3d object and outputs to vs console
#define NAME_D3D12_OBJECT(obj, name) obj->SetName(name); OutputDebugString(L"::D3D12 Object Created: "); OutputDebugString(name); OutputDebugString(L"\n");
#define NAME_D3D12_OBJECT_INDEXED(obj, n, name) \
{ \
    wchar_t fullName[128]; \
    if(swprintf_s(fullName, L"%s[%u]", name, n) > 0) \
    { \
        obj->SetName(fullName); \
        OutputDebugString(L"::D3D12 Object Created: "); \
        OutputDebugString(fullName); \
        OutputDebugString(L"\n"); \
    } \
}
#else
#define NAME_D3D12_OBJECT(obj, name)
#define NAME_D3D12_OBJECT_INDEXED(obj, n, name)
#endif // _DEBUG

}