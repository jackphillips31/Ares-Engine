#include <Windows.h>
#include "entry_point/entry_point.h"
#include "entry_point/platform.h"

#if AR_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	int argc = 0;
	wchar_t** argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

	// Assume max arguments and string size - open to adjustments
	constexpr int MAX_ARGS = 64;
	constexpr int MAX_ARG_LEN = 256;

	char argvBuffer[MAX_ARGS][MAX_ARG_LEN] = {};
	char* argv[MAX_ARGS] = {};

	int actualArgc = 0;

	for (int i = 0; i < argc && i < MAX_ARGS; i++)
	{
		int len = WideCharToMultiByte(
			CP_UTF8, 0, argvW[i], -1, argvBuffer[i], MAX_ARG_LEN, nullptr, nullptr
		);

		if (len > 0)
		{
			argv[actualArgc++] = argvBuffer[i];
		}
	}

	// Free what CommandLineToArgvW allocates internally
	LocalFree(argvW);

	return ares_main(actualArgc, argv);
}
#endif