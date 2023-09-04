#include "ConsoleLog.h"
#include "DX_Interface/DX_Device.h"
#include "DX_Interface/DX_Exception.h"
#include "App.h"

using namespace CGE;

int main()
{
	try
	{
		const auto& device = DX_Device::GetInstance().GetDevice();
		CGE::App engine;
		engine.Run();
		return 0;
	}
	//[todo] clean up and flush gpu
	catch (const DX_Exception& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}