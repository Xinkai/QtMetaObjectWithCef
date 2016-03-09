#include <iostream>

#include <QFile>

#include "cg/exports.h"

#include "AsyncMethodHandler.h"
#include "RenderProcessHandler.h"


RenderProcessHandler::RenderProcessHandler() {

}

RenderProcessHandler::~RenderProcessHandler() {

}

void RenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefV8Context> context) {
    // Retrieve the context's window object.
    CefRefPtr<CefV8Value> object = context->GetGlobal();

    CefRefPtr<CefV8Handler> handler = new AsyncMethodHandler(browser);

    for (auto i = EXPORTED_FUNCTIONS.cbegin(); i != EXPORTED_FUNCTIONS.cend(); i++) {
        CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(*i, handler);
        object->SetValue(*i,
                         func,
                         V8_PROPERTY_ATTRIBUTE_NONE);
    }
}

bool RenderProcessHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                    CefProcessId source_process,
                                                    CefRefPtr<CefProcessMessage> message) {
    const auto& messageName = message->GetName();
    if (messageName == "SignalEmit") {
        CefRefPtr<CefFrame> frame = browser->GetMainFrame();

        // [signalName: str, resultArray: jsonStr]
        const auto arguments = message->GetArgumentList();
        std::string exec = "onSignalEmit(\"" +
                           arguments->GetString(0).ToString() + "\", " +
                           arguments->GetString(1).ToString() + ");";
        LOG(INFO) << "EXEC " << exec;
        frame->ExecuteJavaScript(exec, frame->GetURL(), 0);
        return true;
    }

    if (messageName == "CallReturn") {
        CefRefPtr<CefFrame> frame = browser->GetMainFrame();

        // [callId: int, resultArray: jsonStr]
        const auto arguments = message->GetArgumentList();
        std::ostringstream exec;
        exec << "onCallReturn(" <<
        arguments->GetInt(0) << ", " <<
        arguments->GetString(1).ToString() << ");";
        LOG(INFO) << "EXEC " << exec.str();
        frame->ExecuteJavaScript(exec.str(), frame->GetURL(), 0);
        return true;
    }

    if (messageName == "CallFailed") {
        CefRefPtr<CefFrame> frame = browser->GetMainFrame();

        // [callId: int, resultArray: jsonStr]
        const auto arguments = message->GetArgumentList();
        std::ostringstream exec;
        exec << "onCallFailed(" <<
        arguments->GetInt(0) << ", " <<
        arguments->GetString(1).ToString() << ");";
        LOG(INFO) << "EXEC " << exec.str();
        frame->ExecuteJavaScript(exec.str(), frame->GetURL(), 0);
        return true;
    }

    return false;
}

void RenderProcessHandler::OnWebKitInitialized() {
    QFile extensionFile(":/cef/extension.js");
    const auto ok = extensionFile.open(QFile::ReadOnly | QFile::Text);
    if (ok) {
        const auto extensionCode = QString(extensionFile.readAll()).toStdString();
        CefRegisterExtension("cef/bridge", extensionCode, NULL);
    }
}


