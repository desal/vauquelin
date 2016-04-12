#ifndef VAUQUELIN_V8_
#define VAUQUELIN_V8_

#include "include/cef_base.h"
#include "include/cef_app.h"

class VauquelinV8Handler : public CefV8Handler {
public:
	VauquelinV8Handler() {}

	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(VauquelinV8Handler);
};

#endif