#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include <stdexcept>
#include <cassert>
#include "../CommonMacros.h"
#include "../ConsoleLog.h"
#include "DX_Exception.h"
#include "../RHI/intrusive_ptr.h"

namespace wrl = Microsoft::WRL;

#define AZ_DX12_REFCOUNTED(DXTypeName) \
    namespace CGE \
    { \
        template <> \
        struct IntrusivePtrCountPolicy<DXTypeName> \
        { \
            static void add_ref(DXTypeName* p) { p->AddRef(); } \
            static void release(DXTypeName* p) { p->Release(); } \
        }; \
    }

using IDXGIFactoryX = IDXGIFactory7;
using IDXGIAdapterX = IDXGIAdapter4;
using ID3D12DeviceX = ID3D12Device8;

AZ_DX12_REFCOUNTED(IDXGIFactoryX);
AZ_DX12_REFCOUNTED(IDXGIAdapterX);
AZ_DX12_REFCOUNTED(ID3D12DeviceX);

#ifndef LOCAL_HR
#define LOCAL_HR HRESULT hr;
#endif // !LOCAL_HR

#ifndef DX_THROW_FAILED
#define DX_THROW_FAILED(hrCall) if(FAILED(hr = hrCall)) ThrowFailed(__LINE__, __FILE__, hr);
#endif // !DX_EXEPTION

inline void ThrowFailed(int line, const char* file, HRESULT hr)
{
    if (hr == DXGI_ERROR_DEVICE_REMOVED)
        throw CGE::DX12::DX_DeviceRemovedException(line, file, hr);
    else
        throw CGE::DX12::DX_Exception(line, file, hr);
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