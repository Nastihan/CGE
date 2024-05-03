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

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

namespace wrl = Microsoft::WRL;

// This will call addref and release on the underlying dx interface object
// This will allow us the tranfer from ComPtr to intrusive_ptr
#define DX12_REFCOUNTED(DXTypeName) \
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
using IDXGISwapChainX = IDXGISwapChain4;
using DXGI_SWAP_CHAIN_DESCX = DXGI_SWAP_CHAIN_DESC1;

using ID3D12DeviceX = ID3D12Device8;
using ID3D12CommandQueueX = ID3D12CommandQueue;
using ID3D12GraphicsCommandListX = ID3D12GraphicsCommandList4;

DX12_REFCOUNTED(IDXGIFactory);
DX12_REFCOUNTED(IDXGIFactory1);
DX12_REFCOUNTED(IDXGIFactory2);
DX12_REFCOUNTED(IDXGIFactory3);
DX12_REFCOUNTED(IDXGIFactory4);
DX12_REFCOUNTED(IDXGIFactory5);
DX12_REFCOUNTED(IDXGIFactory6);
DX12_REFCOUNTED(IDXGIFactory7);

DX12_REFCOUNTED(IDXGIAdapter);
DX12_REFCOUNTED(IDXGIAdapter1);
DX12_REFCOUNTED(IDXGIAdapter2);
DX12_REFCOUNTED(IDXGIAdapter3);
DX12_REFCOUNTED(IDXGIAdapter4);

DX12_REFCOUNTED(IDXGISwapChain);
DX12_REFCOUNTED(IDXGISwapChain1);
DX12_REFCOUNTED(IDXGISwapChain2);
DX12_REFCOUNTED(IDXGISwapChain3);
DX12_REFCOUNTED(IDXGISwapChain4);

DX12_REFCOUNTED(ID3D12Device);
DX12_REFCOUNTED(ID3D12Device5);
DX12_REFCOUNTED(ID3D12Device8);

DX12_REFCOUNTED(ID3D12CommandQueue);
DX12_REFCOUNTED(ID3D12CommandAllocator);
DX12_REFCOUNTED(ID3D12GraphicsCommandList);
DX12_REFCOUNTED(ID3D12GraphicsCommandList1);
DX12_REFCOUNTED(ID3D12Fence);

DX12_REFCOUNTED(ID3D12Object);
DX12_REFCOUNTED(ID3D12Resource);

DX12_REFCOUNTED(ID3D12RootSignature);


#ifndef LOCAL_HR
#define LOCAL_HR HRESULT hr;
#endif // !LOCAL_HR

#ifndef DX_THROW_FAILED
#define DX_THROW_FAILED(hrCall) if(FAILED(hr = hrCall)) ThrowFailed(__LINE__, __FILE__, hr);
#endif // !DX_EXEPTION

namespace CGE
{
    namespace DX12
    {
        bool DXAssertSuccess(HRESULT hr);
        DXGI_FORMAT GetBaseFormat(DXGI_FORMAT defaultFormat);
        DXGI_FORMAT GetSRVFormat(DXGI_FORMAT defaultFormat);
        DXGI_FORMAT GetUAVFormat(DXGI_FORMAT defaultFormat);
        DXGI_FORMAT GetDSVFormat(DXGI_FORMAT defaultFormat);
        DXGI_FORMAT GetStencilFormat(DXGI_FORMAT defaultFormat);
    }
}

namespace CGE
{
    namespace DX12
    {
        namespace DX_Alignment
        {
            enum
            {
                Buffer = 16,
                Constant = 256,
                Image = 512,
                CommittedBuffer = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
            };
        }
    }
}

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

