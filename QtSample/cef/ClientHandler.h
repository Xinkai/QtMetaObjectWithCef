#ifndef CEF_CLIENTHANDLER_H
#define CEF_CLIENTHANDLER_H

#include "include/cef_client.h"

class ClientApp;

class ClientHandler: public CefClient,
                     public CefDisplayHandler,
                     public CefLifeSpanHandler,
                     public CefLoadHandler,
                     public CefContextMenuHandler {
public:
    explicit ClientHandler(ClientApp*);

    // Implement CefClient:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;

    // Implement CefDisplayHandler:
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title) override;

    // Implement CefLifeSpanHandler:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    virtual bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override {
      return this;
  }

  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) override {}

  ///
  // Called to allow custom display of the context menu. |params| provides
  // information about the context menu state. |model| contains the context menu
  // model resulting from OnBeforeContextMenu. For custom display return true
  // and execute |callback| either synchronously or asynchronously with the
  // selected command ID. For default display return false. Do not keep
  // references to |params| or |model| outside of this callback.
  ///
  /*--cef()--*/
  virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params,
                              CefRefPtr<CefMenuModel> model,
                              CefRefPtr<CefRunContextMenuCallback> callback) override {
      return true;
  }

  ///
  // Called to execute a command selected from the context menu. Return true if
  // the command was handled or false for the default implementation. See
  // cef_menu_id_t for the command ids that have default implementations. All
  // user-defined command ids should be between MENU_ID_USER_FIRST and
  // MENU_ID_USER_LAST. |params| will have the same values as what was passed to
  // OnBeforeContextMenu(). Do not keep a reference to |params| outside of this
  // callback.
  ///
  /*--cef()--*/
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) override { return false; }

  ///
  // Called when the context menu is dismissed irregardless of whether the menu
  // was empty or a command was selected.
  ///
  /*--cef()--*/
  virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame) override {}

    IMPLEMENT_REFCOUNTING(ClientHandler);

private:
    ClientApp* clientApp = nullptr;
};

#endif // CEF_CLIENTHANDLER_H
