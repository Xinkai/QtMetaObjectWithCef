#include <thread>

#include <QApplication>
#include <QDebug>

#include "cef/Shell.h"
#include "cef/ClientApp.h"

#include "include/wrapper/cef_helpers.h"

bool isSubProcess(const int& argc, char** argv) {
  for (int i = 0; i < argc; i++) {
    constexpr auto length = strlen("--type=");
    if (strncmp(argv[i], "--type=", length) == 0) {
      return true;
    };
  }
  return false;
}

int main(int argc, char *argv[]) {
  CefScopedArgArray scoped_arg_array(argc, argv);
  char** argv_copy = scoped_arg_array.array();

  Shell* shell = nullptr;
  if (!isSubProcess(argc, argv)) {
    // Note: Only initialize QApplication if this is not a Chromium subprocess
    shell = new Shell(argc, argv_copy);
  };

  // Provide CEF with command-line arguments.
  CefMainArgs main_args(argc, argv);

  // SimpleApp implements application-level callbacks. It will create the first
  // browser instance in OnContextInitialized() after CEF has initialized.
  CefRefPtr<ClientApp> app(new ClientApp());

  // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
  // that share the same executable. This function checks the command-line and,
  // if this is a sub-process, executes the appropriate logic.
  int exit_code = CefExecuteProcess(main_args, app.get(), NULL);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

  // Specify CEF global settings here.
  CefSettings settings;

  // Initialize CEF for the browser process.
  CefInitialize(main_args, settings, app.get(), NULL);

  qDebug() << "Main PID" << getpid();

  if (shell) {
    QObject::connect(shell, &QApplication::aboutToQuit, []() {
      qDebug() << "Cef Shutting down";
      CefShutdown();
    });
  }

  int result = 0;
  if (shell) {
    result = shell->exec();
  } else {
    CefRunMessageLoop();
  }
  return result;
}
