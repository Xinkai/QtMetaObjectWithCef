#include <iostream>
#include <QDebug>
#include <QString>

#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "include/base/cef_logging.h"
#include "include/cef_app.h"
#include "include/cef_v8.h"

#include "include/wrapper/cef_helpers.h"

#include "ClientApp.h"
#include "ClientHandler.h"
#include "Shell.h"
#include "bridge/Bridge.h"


ClientHandler::ClientHandler(ClientApp* clientApp) {
    this->clientApp = clientApp;
}

CefRefPtr<CefLifeSpanHandler> ClientHandler::GetLifeSpanHandler() {
    return this;
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
    CEF_REQUIRE_UI_THREAD();

    std::string titleStr(title);
    std::cout << "title changed to %s" << titleStr;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    const auto wId = browser->GetHost()->GetWindowHandle();
    LOG(INFO) << "BrowserCreated " << wId << "  " << getpid();

    const auto bridge = static_cast<Shell*>(qApp)->bridge;
    if (!bridge) {
        return;
    }

//    this->clientApp->windowing->onBrowserWindowCreated(browser);
    #include "cg/ClientSignal.cppf"

}

bool ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                             CefProcessId source_process,
                                             CefRefPtr<CefProcessMessage> message) {
    // Check the message name.
    const std::string& message_name = message->GetName();

    if (message_name == "MethodCall") {
        const auto arguments = message->GetArgumentList();

        const auto methodName = arguments.get()->GetString(0).ToString();
        const auto json = arguments.get()->GetString(1).ToString();
        const auto callId = arguments.get()->GetInt(2);

        std::cout << "MethodCall id " << callId << " " << methodName << " " << json << std::endl;
        // Handle the message here...

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "JSON Error:" << parseError.errorString();
            return false;
        }
        const auto argsArray = jsonDoc.array();

        const auto bridge = static_cast<Shell*>(qApp)->bridge;

        std::vector<std::string> attempts;

        #include "cg/ClientSlot.cppf"

        if (!attempts.empty()) {
            const char* const delim = "\n";

            std::ostringstream imploded;
            std::copy(attempts.begin(), attempts.end(),
                      std::ostream_iterator<std::string>(imploded, delim));
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("CallFailed");
            CefRefPtr<CefListValue> args = msg->GetArgumentList();

            args->SetInt(0, callId);

            // To keep the string version of the JSON value escaped
            // here to use the functionality provided by QDebug
            QString tmpStr;
            QDebug(&tmpStr) << QString::fromStdString(imploded.str()); // Have to cast to QString first, otherwise mojibake
            args->SetString(1, tmpStr.toStdString());

            browser->SendProcessMessage(PID_RENDERER, msg);
            return true;
        }

        return false;
    }
    return false;
}
