#ifndef VAUQUELIN_FAVICON_H_
#define VAUQUELIN_FAVICON_H_

#include "vauquelin/vauquelin_handler.h"

#include "include/cef_urlrequest.h"

class FaviconRequestClient : public CefURLRequestClient {
public:
	FaviconRequestClient(CefRefPtr<VauquelinHandler> handler, CefRefPtr<CefBrowser> browser)
		: upload_total_(0),
		download_total_(0),
		handler_(handler),
		browser_(browser) {}

	virtual void OnRequestComplete(CefRefPtr<CefURLRequest> request) OVERRIDE;

	virtual void OnUploadProgress(CefRefPtr<CefURLRequest> request,
		int64 current, int64 total) OVERRIDE;

	virtual void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
		int64 current,
		int64 total) OVERRIDE;

	virtual void OnDownloadData(CefRefPtr<CefURLRequest> request,
		const void* data, size_t data_length) OVERRIDE;

	virtual bool GetAuthCredentials(bool isProxy,
		const CefString& host, int port,
		const CefString& realm,
		const CefString& scheme,
		CefRefPtr< CefAuthCallback > callback) OVERRIDE;

private:
	uint64 upload_total_;
	uint64 download_total_;
	std::string download_data_;
	CefRefPtr<VauquelinHandler> handler_;
	CefRefPtr<CefBrowser> browser_;

private:
	IMPLEMENT_REFCOUNTING(FaviconRequestClient);
};

#endif //VAUQUELIN_FAVICON_H_