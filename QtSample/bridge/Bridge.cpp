#include <QDebug>

#include "cef/Shell.h"

#include "Bridge.h"


Bridge::Bridge(Shell* parent) : QObject(parent) {
    this->subModule = new SubModule(this);
}

int Bridge::returnNumber(const QString& name) {
    qDebug() << "returnNumber" << name;
    return 0;
}

QVariantMap Bridge::returnObject(const QVariantMap& options) {
    qDebug() << "returnObject called with" << options;
    QVariantMap result;
    result["success"] = 1;
    result["error"] = "NoError";
    return result;
}




