#include "listener.h"
#include <windows.h>
#include <QDebug>

Listener::Listener(QObject *parent) : QObject(parent)
{
    m_stop = false;
}

void Listener::process()
{
    //int n = 0;
    while(true)
    {
        emit quant();
        Sleep(10);
        if (m_stop)
            break;
    }
}

void Listener::stop()
{
    m_stop = true;
}
