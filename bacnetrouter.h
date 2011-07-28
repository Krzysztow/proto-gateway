#ifndef BACNETROUTER_H
#define BACNETROUTER_H

#include <QObject>

class BacnetRouter :
        public QObject//inherits QObject, so that QTimer events may be received
{
    Q_OBJECT
public:
    explicit BacnetRouter(QObject *parent = 0);

signals:

public slots:

};

#endif // BACNETROUTER_H
