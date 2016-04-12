#include "vauquelin/vauquelin_v8.h"

#include <shellapi.h>

bool VauquelinV8Handler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) {

	if (name == "ShellExecute" && arguments.size() == 3 && arguments[0]->IsString()) {
		std::wstring file = arguments[0]->GetStringValue().ToWString();
		std::wstring parameters;
		std::wstring directory;
		const wchar_t *ptr_parameters = 0;
		const wchar_t *ptr_directory = 0;

		if (arguments[1]->IsString()) {
			parameters = arguments[1]->GetStringValue().ToWString();
			ptr_parameters = parameters.c_str();
		}

		if (arguments[2]->IsString()) {
			directory = arguments[1]->GetStringValue().ToWString();
			ptr_directory = directory.c_str();
		}

		::ShellExecuteW(NULL, NULL, file.c_str(), ptr_parameters, ptr_directory, SW_SHOWNORMAL);
		return true;
	}

	// Function does not exist.
	return false;
}