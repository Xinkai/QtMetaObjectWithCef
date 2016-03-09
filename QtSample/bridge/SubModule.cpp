#include <QDebug>
#include <QTimer>
#include "SubModule.h"

SubModule::SubModule(QObject* parent) : QObject(parent) {
    const auto timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, [this]() {
        emit this->timeout(true);
    });
    timer->start(1000);
}

SubModule::~SubModule() {

}

void SubModule::setType(const MyEnumType& type) {
    qDebug() << "setType" << type;
}






