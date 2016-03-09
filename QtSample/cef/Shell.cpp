#include <thread>

#include <iostream>
#include <QDebug>

#include "include/cef_app.h"
#include <unistd.h>

#include "Shell.h"
#include "bridge/Bridge.h"

using namespace std::literals;

Shell::Shell(int& argc, char** argv) : QApplication(argc, argv) {
  this->bridge = new Bridge(this);
  std::cout << "Shell Pid " << getpid() << std::endl;
}

Shell::~Shell() {

}

int Shell::exec() {
  const auto app = static_cast<Shell*>(qApp);
  while(!app->quitRequested) {
    app->processEvents();
    CefDoMessageLoopWork();
    std::this_thread::sleep_for(1ms);
  }
  CefShutdown();
  emit app->aboutToQuit();
  return 0;
}

void Shell::quit() {
  const auto app = static_cast<Shell*>(qApp);
  app->quitRequested = true;
}
