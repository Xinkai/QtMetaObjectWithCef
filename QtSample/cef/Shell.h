#ifndef CEF_SHELL_H
#define CEF_SHELL_H

#include <QApplication>

class Bridge;

class MainWindow;

class Shell : public QApplication {
Q_OBJECT

public:
    explicit Shell(int& argc, char** argv);

    ~Shell();

    MainWindow* mainWindow = nullptr;
    Bridge* bridge = nullptr;

    static int exec();

public Q_SLOTS:

    static void quit();

Q_SIGNALS:

    void aboutToQuit();

private:
    bool quitRequested = false;

};

#endif // CEF_SHELL_H
