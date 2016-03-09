#ifndef CEF_RENDER_PROCESS_HANDLER_H
#define CEF_RENDER_PROCESS_HANDLER_H

#include "include/cef_render_process_handler.h"

class RenderProcessHandler : public CefRenderProcessHandler {

public:
  explicit RenderProcessHandler();
  ~RenderProcessHandler();

  void OnContextCreated(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context);

  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message);

  void OnWebKitInitialized();

  IMPLEMENT_REFCOUNTING(RenderProcessHandler);
};


#endif //CEF_RENDER_PROCESS_HANDLER_H
