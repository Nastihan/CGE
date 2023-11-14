#include "DX_CommonHeaders.h"

namespace CGE
{
	namespace DX12
	{
        bool DXAssertSuccess(HRESULT hr)
        {
            if (FAILED(hr))
            {
                char* msgBuf = nullptr;

                if (FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    hr,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&msgBuf,
                    0,
                    NULL))
                {
                    printf("HRESULT not a success %x, error msg = %s", hr, msgBuf);
                    LocalFree(msgBuf);
                }
                else
                {
                    printf("HRESULT not a success %x, Unable to retrieve error msg", hr);
                }
                return false;
            }
            return true;
        }
	}
}