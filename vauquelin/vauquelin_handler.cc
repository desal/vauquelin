#include "vauquelin/vauquelin_handler.h"

#include "vauquelin/vauquelin_favicon.h"
#include "vauquelin/winicon.h"

#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include <Shlwapi.h>

namespace {
	VauquelinHandler* g_instance = NULL;
}  // namespace

std::wstring VauquelinHandler::GetStartupHost() {

	CefRefPtr<CefCommandLine> command_line =
		CefCommandLine::GetGlobalCommandLine();
	CefString startup_url = command_line->GetSwitchValue("url");

	CefURLParts startup_url_parts;
	CefParseURL(startup_url, startup_url_parts);

	CefString startup_host(&startup_url_parts.host);
	return startup_host.ToWString();
}

void VauquelinHandler::WriteConfig(std::map<std::wstring, ::WINDOWPLACEMENT> config) {
	wchar_t folder[1024];

	if (!::ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\vauquelin", folder, 1023)) {
		::MessageBoxW(0, L"Config failure", L"ERROR", 0);
		return;
	}

	if (!::CreateDirectoryW(folder, NULL) && ::GetLastError() != ERROR_ALREADY_EXISTS) {
		::MessageBoxW(0, L"Config failure", L"ERROR", 0);
		return;
	}

	std::ofstream ofs;
	ofs.open(std::wstring(folder) + L"\\windowpos.dat", std::ios::binary);
	size_t size = config.size();
	ofs.write((char*)&size, sizeof(size));
	for (auto it = config.begin(); it != config.end(); it++) {
		size = it->first.size() * sizeof(wchar_t);
		ofs.write((char*)&size, sizeof(size));
		ofs.write((char*)it->first.c_str(), size);
		ofs.write((char*)&it->second, sizeof(it->second));
	}
	ofs.close();
}

std::map<std::wstring, ::WINDOWPLACEMENT> VauquelinHandler::ReadConfig() {
	std::map<std::wstring, ::WINDOWPLACEMENT> result;

	wchar_t file[1024];

	if (!::ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\vauquelin\\windowpos.dat", file, 1023)) {
		::MessageBoxW(0, L"Config failure", L"ERROR", 0);
		return result;
	}

	if (!::PathFileExists(file))
		return result;

	std::ifstream ifs;
	ifs.open(std::wstring(file), std::ios::binary);
	size_t num_entries;
	ifs.read((char*)&num_entries, sizeof(num_entries));
	for (int i = 0; i < num_entries; i++) {
		size_t string_size;
		ifs.read((char*)&string_size, sizeof(string_size));

		char* buf = (char*)malloc(string_size);
		ifs.read(buf, string_size);
		std::wstring key;
		key.assign((wchar_t*)buf, string_size >> 1);

		::WINDOWPLACEMENT val;
		ifs.read((char*)&val, sizeof(val));

		result[key] = val;

	}
	return result;
}

VauquelinHandler::VauquelinHandler()
	: is_closing_(false) {
	DCHECK(!g_instance);
	g_instance = this;
}

VauquelinHandler::~VauquelinHandler() {
	g_instance = NULL;
}

// static
VauquelinHandler* VauquelinHandler::GetInstance() {
	return g_instance;
}

void VauquelinHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	//Only load first window position for now
	//TODO (maybe) think about if i want to do something useful for secondary window loading
	//some sort of save by tag might be useful, but then i'd to enrich window.open to add a tag method

	if (browser_list_.empty()) {
		std::wstring startup_host = GetStartupHost();
		std::map<std::wstring, ::WINDOWPLACEMENT> config = ReadConfig();
		if (config.count(startup_host)) {
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			::WINDOWPLACEMENT window_placement = config[startup_host];
			::SetWindowPlacement(hwnd, &window_placement);
		}
	}

	// Add to the list of existing browsers.
	browser_list_.push_back(browser);
}

bool VauquelinHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void VauquelinHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}

	int id = browser->GetIdentifier();
	//TODO panic out here if this is false
	if (browser_state_.count(id))
		browser_state_.erase(id);

	if (browser_list_.empty()) {
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		::WINDOWPLACEMENT window_placement;
		::GetWindowPlacement(hwnd, &window_placement);

		std::wstring startup_host = GetStartupHost();
		std::map<std::wstring, ::WINDOWPLACEMENT> config = ReadConfig();
		config[startup_host] = window_placement;
		WriteConfig(config);

		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

void VauquelinHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void VauquelinHandler::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&VauquelinHandler::CloseAllBrowsers, this, force_close));
		return;
	}

	if (browser_list_.empty())
		return;

	BrowserList::const_iterator it = browser_list_.begin();
	for (; it != browser_list_.end(); ++it)
		(*it)->GetHost()->CloseBrowser(force_close);
}

void VauquelinHandler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
	const std::vector<CefString>& iconURLs) {

	if (iconURLs.size() == 0)
		return;

	int id = browser->GetIdentifier();
	if (browser_state_.count(id) && browser_state_[id].LoadedFaviconURL == iconURLs[0].ToWString())
		return;

	browser_state_[id].LoadedFaviconURL = iconURLs[0].ToWString();

	CefRefPtr<CefRequest> request = CefRequest::Create();
	CefRefPtr<FaviconRequestClient> client = new FaviconRequestClient(CefRefPtr<VauquelinHandler>(this), browser);
	request->SetURL(iconURLs[0]);
	request->SetMethod("GET");
	CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client.get(), browser->GetHost()->GetRequestContext());
}

int iconOffset(const char* icon_bytes, size_t icon_size, int desired_width, int desired_height) {
	ICONDIR* icon_dir = (ICONDIR*)icon_bytes;

	size_t header_size = sizeof(icon_dir);
	size_t extra_entry_size = (icon_dir->idCount - 1) * sizeof(ICONDIRENTRY);

	if (icon_dir->idReserved != 0 || icon_dir->idType != 1 || header_size + extra_entry_size > icon_size)
		return 0;

	int best_entry = 0;
	int best_width = 0;
	int best_height = 0;

	for (int i = 0; i < icon_dir->idCount; i++) {
		int width = int(icon_dir->idEntries[i].bWidth);
		int height = int(icon_dir->idEntries[i].bHeight);

		if ((width == desired_width && height == desired_height) || (best_width == 0 && width > desired_width && height > desired_height)) {
			best_width = width;
			best_height = height;
			best_entry = i;
		}
	}
	return icon_dir->idEntries[best_entry].dwImageOffset;

}

void VauquelinHandler::UpdateIcon(CefRefPtr<CefBrowser> browser, std::string icon_bytes) {
	if (!icon_bytes.size())
		return;

	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	PBYTE ptr_byte = (PBYTE)icon_bytes.c_str();

	int small_icon_size = ::GetSystemMetrics(SM_CXSMICON);
	int large_icon_size = ::GetSystemMetrics(SM_CXICON);

	int offset_small = iconOffset(icon_bytes.c_str(), icon_bytes.size(), small_icon_size, small_icon_size);
	int offset_large = iconOffset(icon_bytes.c_str(), icon_bytes.size(), large_icon_size, large_icon_size);

	if (offset_small) {
		BITMAPINFOHEADER* img = (BITMAPINFOHEADER*)(ptr_byte + offset_small);
		HICON hicon = ::CreateIconFromResource((BYTE*)img, img->biSize + img->biSizeImage, true, 0x00030000);
		if (hicon) {
			HICON prev = (HICON)::SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
			if (prev)
				::DestroyIcon(prev);
		}
	}
	if (offset_large) {
		BITMAPINFOHEADER* img = (BITMAPINFOHEADER*)(ptr_byte + offset_large);
		HICON hicon = ::CreateIconFromResource((BYTE*)img, img->biSize + img->biSizeImage, true, 0x00030000);
		if (hicon) {
			HICON prev = (HICON)::SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);
			if (prev)
				::DestroyIcon(prev);
		}
	}
}
