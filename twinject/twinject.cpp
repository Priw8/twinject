
#include <windows.h>
#include <detours.h>
#include <tchar.h>
#include <psapi.h>
#include <strsafe.h>
#include <atlstr.h>

#include "debugger.h"

STARTUPINFOA si;
PROCESS_INFORMATION pi;

const char *ininame = "twinject.ini";

#define TH08_LOADER

int main(const int argc, const char *argv[])
{
	// The following block of code defines debugging paths to the games I have in my debug environment.
	// TODO use dynamic configuration based loading instead of hard coding everything
#if defined(TH07_LOADER)
	_putenv("th=th07");
	char *exepath = "D:\\Programming\\Multi\\th07\\th07.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th07";
#elif defined(TH08_LOADER)
	_putenv("th=th08");
	char *exepath = "D:\\Programming\\Multi\\th08\\th08.exe";
	char *dllpath = "D:\\Programming\\Multi\\twinject\\Release\\twinhook.dll";
	char *currentdir = "D:\\Programming\\Multi\\th08";
#else
	// The following code loads configuration data from an external file
	configuration config;

	if (ini_parse(ininame, handler, &config) < 0) {
		printf("failed to load configuration %s\n", ininame);
		return 1;
	}

	char exepath[MAX_PATH];
	char dllpath[MAX_PATH];
	char currentdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentdir);
	PathCombine(exepath, currentdir, config.exename);
	PathCombine(dllpath, currentdir, config.dllname);
#endif
	
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	if (DetourCreateProcessWithDll(exepath, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | DEBUG_PROCESS, NULL,
		currentdir, &si, &pi, dllpath, NULL))
	{
		printf("twinject: Injection OK\n");
	}
	else
	{
		printf("twinject: Injection Fail\n");
	}

	DEBUG_EVENT debugEv;
	EnterDebugLoop(&debugEv);

	return 0;

}

