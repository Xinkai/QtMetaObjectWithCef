#ifndef CEF_SUBMODULE_H
#define CEF_SUBMODULE_H

#include <QObject>

enum MyEnumType {
    One,
    Two,
};

class SubModule : public QObject {
Q_OBJECT

public:
    explicit SubModule(QObject* parent = nullptr);
    ~SubModule();

public slots:
    void setType(const MyEnumType& type);

signals:
    void timeout(const bool boolean);

};


#endif //CEF_SUBMODULE_H
