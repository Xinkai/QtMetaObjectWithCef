#include <QTextStream>
#include <QDebug>
#include <QApplication>
#include <QMetaProperty>

#include "bridge/Bridge.h"

void getMeta(const QObject* object, QTextStream& output) {
    const auto metaObject = object->metaObject();
    // properties
    const auto propertyCount = metaObject->propertyCount();
    for (int i = 0; i < propertyCount; i++) {
        const auto property = metaObject->property(i);

        if (QString(property.typeName()).endsWith("*")) {
            const auto propertyObject = object->property(property.name());
            getMeta(qvariant_cast<QObject*>(propertyObject), output);
            output << "Object" << " "
            << property.typeName() << " "
            << qPrintable(QString(metaObject->className()) + "::" + property.name())
            << endl;
        } else {
            output << "Property" << " "
            << property.typeName() << " "
            << qPrintable(QString(metaObject->className()) + "::" + property.name())
            << endl;
        }
    }

    // methods
    const auto methodCount = metaObject->methodCount();
    for (int i = 0; i < methodCount; i++) {
        const auto method = metaObject->method(i);
        switch (method.methodType()) {
            case QMetaMethod::Slot: {
                if (method.access() == QMetaMethod::Public) {
                    output << "Slot" << " "
                    << method.typeName() << " "
                    << qPrintable(QString(metaObject->className()) + "::" + method.methodSignature().constData())
                    << endl;
                }
                break;
            }
            case QMetaMethod::Signal: {
                output << "Signal" << " "
                << method.typeName() << " "
                << qPrintable(QString(metaObject->className()) + "::" + method.methodSignature().constData())
                << endl;
                break;
            }
            default: {

            }
        }
    }
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    const auto handler = new Bridge(nullptr);

    QString str;
    QTextStream output(&str);
    getMeta(handler, output);

    qDebug() << qPrintable(str);

    return 0;
}