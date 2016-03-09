#ifndef CEF_SCHEMEHANDLERFACTORY_H
#define CEF_SCHEMEHANDLERFACTORY_H

#include "include/wrapper/cef_stream_resource_handler.h"

// Implementation of the factory for creating client request handlers.
class SchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
  virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                               CefRefPtr<CefFrame> frame,
                                               const CefString& scheme_name,
                                               CefRefPtr<CefRequest> request)
                                               override;

  IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
};


#endif // CEF_SCHEMEHANDLERFACTORY_H
