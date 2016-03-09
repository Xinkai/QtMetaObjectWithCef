#include <iostream>
#include "ClientApp.h"
#include "ClientHandler.h"
#include "RenderProcessHandler.h"
#include "SchemeHandlerFactory.h"

#include "include/wrapper/cef_helpers.h"

ClientApp::ClientApp() {

}

void ClientApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    LOG(INFO) << "CreateBrowser PID " << getpid();

    // register custom scheme
    const auto schemeHandlerFactory = new SchemeHandlerFactory();
    CefRegisterSchemeHandlerFactory("app", "app", schemeHandlerFactory);

    // create window
    CefWindowInfo window_info;

    // ClientHandler implements browser-level callbacks.
    CefRefPtr<ClientHandler> handler(new ClientHandler(this));

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    const std::string url = "app://app/index.html";

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(window_info, handler.get(), url,
                                  browser_settings, NULL);
}

void ClientApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
    registrar->AddCustomScheme("app", true, false, false);
}

CefRefPtr<CefBrowserProcessHandler> ClientApp::GetBrowserProcessHandler() {
    return this;
}

CefRefPtr<CefRenderProcessHandler> ClientApp::GetRenderProcessHandler() {
    return new RenderProcessHandler();
}
