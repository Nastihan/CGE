#include "ConsoleLog.h"
#include "DX_Interface/DX_Device.h"
#include "DX_Interface/DX_Exception.h"
#include "App.h"

using namespace CGE::DX12;

int main()
{
	try
	{
		CGE::App engine;
		engine.Run();
		return 0;
	}
	//[todo] clean up and flush gpu
	catch (const DX_Exception& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}