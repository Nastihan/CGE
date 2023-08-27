#include "ConsoleLog.h"
#include "DX_Interface/DX_Device.h"
#include "VK_Interface/VK_Device.h"
#include "App.h"

using namespace CGE;

int main()
{
	
	try
	{
		//const auto& device = DX_Device::GetInstance().GetDevice();
		const auto& vkI = VK_Device::GetInstance();
		CGE::App{}.Run();
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_SETFOREGROUND);
	}
	return 0;
}