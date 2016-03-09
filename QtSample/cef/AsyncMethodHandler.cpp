#include "AsyncMethodHandler.h"

AsyncMethodHandler::AsyncMethodHandler(CefRefPtr<CefBrowser> browser) {
    this->browser = browser;
}

bool AsyncMethodHandler::Execute(const CefString& name,
                                 CefRefPtr<CefV8Value> object,
                                 const CefV8ValueList& arguments,
                                 CefRefPtr<CefV8Value>& retval,
                                 CefString& exception) {
  if (1 != arguments.size())  {
    exception = "Argument count mismatch, expecting 1";
    return true;
  }
  const auto value = arguments.at(0);
  if (!value->IsString()) {
    exception = "Argument 0 should be a string.";
    return true;
  }
  const auto valueStr = value->GetStringValue();

  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("MethodCall");
  CefRefPtr<CefListValue> args = msg->GetArgumentList();

  args->SetString(0, name);
  args->SetString(1, valueStr);

  const auto thisCallId = this->callId++;
  args->SetInt(2, thisCallId);

  this->browser->SendProcessMessage(PID_BROWSER, msg);
  retval = CefV8Value::CreateUInt(thisCallId);

  return true;
}
