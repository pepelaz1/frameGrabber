#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>

class Listener : public QObject
{
    Q_OBJECT
public:
    explicit Listener(QObject *parent = 0);
    void stop();
signals:
    void quant();
private:
    bool m_stop;

public slots:
    void process();
};

#endif // LISTENER_H
