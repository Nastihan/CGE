#include "ConsoleLog.h"
#include "DX_Interface/DX_Device.h"
#include "VK_Interface/VK_Device.h"
#include "App.h"

using namespace CGE;

int main()
{
	const auto& device = DX_Device::GetInstance().GetDevice();
	const auto& vkI = VK_Device::GetInstance();
	CGE::App engine;
	engine.Run();
	return 0;
}