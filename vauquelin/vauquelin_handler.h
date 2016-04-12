#ifndef VAUQUELIN_HANDLER_H_
#define VAUQUELIN_HANDLER_H_

#include "include/cef_client.h"

#include <list>

class VauquelinHandler : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler {
public:
	VauquelinHandler();
	~VauquelinHandler();

	// Provide access to the single global instance of this object.
	static VauquelinHandler* GetInstance();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
		return this;
	}

	// CefDisplayHandler methods:
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) OVERRIDE;

	virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
		const std::vector<CefString>& iconURLs) OVERRIDE;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsClosing() const { return is_closing_; }

	void UpdateIcon(CefRefPtr<CefBrowser> browser, std::string icon_bytes);

private:
	//TODO Use this for config key
	struct ConfigKey {
		std::wstring Host;
		std::wstring Tag;
	};

	//TODO replace loaded_favicon_urls_ with this:
	//TODO replace BrowserList with this.
	struct BrowserState {
		std::wstring LoadedFaviconURL;
		std::wstring Tag;
	};

	std::wstring GetStartupHost();
	void WriteConfig(std::map<std::wstring, ::WINDOWPLACEMENT> config);
	std::map<std::wstring, ::WINDOWPLACEMENT> ReadConfig();

	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;
	//std::map<int, std::wstring> loaded_favicon_urls_;
	std::map<int, BrowserState> browser_state_;

	bool is_closing_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(VauquelinHandler);
};

#endif  // VAUQUELIN_HANDLER_H_
