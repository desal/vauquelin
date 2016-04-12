#include "vauquelin/vauquelin_favicon.h"

#include "include/cef_urlrequest.h"

void FaviconRequestClient::OnRequestComplete(CefRefPtr<CefURLRequest> request) {
	//TODO I suspect that 404's and the like are making there way through here.

	//CefURLRequest::Status status = request->GetRequestStatus();
	CefURLRequest::ErrorCode error_code = request->GetRequestError();
	//CefRefPtr<CefResponse> response = request->GetResponse();

	if (error_code == 0) {
		handler_->UpdateIcon(browser_, download_data_);
	}
}

void FaviconRequestClient::OnUploadProgress(CefRefPtr<CefURLRequest> request,
	int64 current, int64 total) {
	upload_total_ = total;
}

void FaviconRequestClient::OnDownloadProgress(CefRefPtr<CefURLRequest> request,
	int64 current, int64 total) {
	download_total_ = total;
}

void FaviconRequestClient::OnDownloadData(CefRefPtr<CefURLRequest> request,
	const void* data,
	size_t data_length) {
	download_data_ += std::string(static_cast<const char*>(data), data_length);
}

bool FaviconRequestClient::GetAuthCredentials(bool isProxy, const CefString& host,
	int port, const CefString& realm, const CefString& scheme,
	CefRefPtr< CefAuthCallback > callback) {

	return true;
}