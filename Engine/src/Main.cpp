#include "ConsoleLog.h"
#include "DX_Interface/DX_Device.h"
#include "App.h"

using namespace CGE;

int main()
{
	const auto& device = DX_Device::GetInstance().GetDevice();
	CGE::App engine;
	engine.Run();
	return 0;
}