#ifndef CHANNEL_CHANNEL_H_
#define CHANNEL_CHANNEL_H_

#include <QObject>
#include <QVariantMap>

#include "SubModule.h"

class Shell;


class Bridge : public QObject {
Q_OBJECT
    Q_PROPERTY(
        SubModule* subModule
        MEMBER subModule
    )
public:
    explicit Bridge(Shell* parent = nullptr);

    SubModule* subModule = nullptr;

public slots:
    int returnNumber(const QString& name);
    QVariantMap returnObject(const QVariantMap& options);

signals:
    void bridgeSignal();

private:

};

#endif
