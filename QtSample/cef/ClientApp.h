#ifndef CEF_CLIENTAPP_H
#define CEF_CLIENTAPP_H

#include "include/cef_app.h"

class ClientApp: public CefApp,
                 public CefBrowserProcessHandler {
public:
  ClientApp();

  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

  virtual void OnContextInitialized() override;

  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

  virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override;

private:

    CefRefPtr<CefRenderProcessHandler> renderProcess = nullptr;

    IMPLEMENT_REFCOUNTING(ClientApp);
};

#endif // CEF_CLIENTAPP_H
