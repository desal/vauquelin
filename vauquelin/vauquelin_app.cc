#include "vauquelin/vauquelin_app.h"

#include "vauquelin/vauquelin_handler.h"
#include "vauquelin/vauquelin_v8.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"

#include <string>

#define SECURITY_WIN32
#include <security.h>
#include <shellapi.h>

VauquelinApp::VauquelinApp() {
}

void VauquelinApp::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

	//From CefSimple:
	CefWindowInfo window_info;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	window_info.SetAsPopup(NULL, "vauquelin");
#endif

	CefRefPtr<VauquelinHandler> handler(new VauquelinHandler());
	CefBrowserSettings browser_settings;

	std::string url;

	//before calling into CEF, this is checked to contain a value.
	CefRefPtr<CefCommandLine> command_line =
		CefCommandLine::GetGlobalCommandLine();
	url = command_line->GetSwitchValue("url");

	CefBrowserHost::CreateBrowser(window_info, handler.get(), url,
		browser_settings, NULL);
}

void VauquelinApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {

	// Retrieve the context's window object.
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	CefRefPtr<CefV8Value> vq = CefV8Value::CreateObject(NULL);
	object->SetValue("vq", vq, V8_PROPERTY_ATTRIBUTE_NONE);

	//Windows Username
	wchar_t winapi_username[64];
	DWORD buf_size = 64;
	::GetUserNameExW(::NameSamCompatible, winapi_username, &buf_size);

	CefRefPtr<CefV8Value> username = CefV8Value::CreateString(CefString(winapi_username));
	vq->SetValue("Username", username, V8_PROPERTY_ATTRIBUTE_NONE);

	//Shell Execute function
	CefRefPtr<CefV8Handler> handler = new VauquelinV8Handler();
	CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("ShellExecute", handler);
	vq->SetValue("ShellExecute", func, V8_PROPERTY_ATTRIBUTE_NONE);
}

bool VauquelinApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	NavigationType navigation_type,
	bool is_redirect) {

	CefString target_url = request->GetURL();
	CefURLParts target_url_parts;

	if (!CefParseURL(target_url, target_url_parts)) //Bad url, not sure if this can happen
		return true;

	//Parse the command line arg.
	CefRefPtr<CefCommandLine> command_line =
		CefCommandLine::GetGlobalCommandLine();
	CefString startup_url = command_line->GetSwitchValue("url");

	CefURLParts startup_url_parts;
	CefParseURL(startup_url, startup_url_parts);

	CefString target_host(&target_url_parts.host);
	CefString startup_host(&startup_url_parts.host);

	if (target_host != startup_host) {
		::ShellExecuteW(NULL, NULL, target_url.ToWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
		return true;
	}

	return false;
}
