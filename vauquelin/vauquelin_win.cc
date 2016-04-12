#include "vauquelin/vauquelin_app.h"

#include "include/cef_parser.h"
#include "include/cef_sandbox_win.h"

#include <sstream>

#include <windows.h>

// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);

	// Specify CEF global settings here.
	CefSettings settings;
	settings.single_process = true;
	settings.no_sandbox = true;

	CefString command_line_string;
	command_line_string.FromWString(::GetCommandLineW());

	CefRefPtr<CefCommandLine> cef_command_line = CefCommandLine::CreateCommandLine();
	cef_command_line->InitFromString(command_line_string);

	CefString url = cef_command_line->GetSwitchValue("url");

	CefURLParts urlParts;

	if (!CefParseURL(url, urlParts)) {
		::MessageBoxW(0, L"Failed to parse url\nYou must specify an url with --url=http://...", L"Error", 0);
		return 0;
	}

	CefRefPtr<VauquelinApp> app(new VauquelinApp);
	CefInitialize(main_args, settings, app.get(), NULL);
	CefRunMessageLoop();
	CefShutdown();

	return 0;
}
