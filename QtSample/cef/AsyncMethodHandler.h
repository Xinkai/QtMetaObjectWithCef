#ifndef CEF_CHANNEL_ADAPTER_H
#define CEF_CHANNEL_ADAPTER_H

#include <string>
#include <list>
#include <QJsonValue>
#include "include/base/cef_logging.h"

#include "include/cef_app.h"
#include "include/cef_v8.h"
#include "Shell.h"
#include "bridge/Bridge.h"

class AsyncMethodHandler : public CefV8Handler {
public:
  AsyncMethodHandler(CefRefPtr<CefBrowser> browser);

  virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) override;

private:
  CefRefPtr<CefBrowser> browser = nullptr;
  uint32 callId = 0;  // assign a callId to each method call

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(AsyncMethodHandler);
};

#endif // CEF_CHANNEL_ADAPTER_H
