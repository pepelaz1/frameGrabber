#ifndef RNDMANAGER_H
#define RNDMANAGER_H

#include "graphmanager.h"

class RndManager : public GraphManager
{
protected:
    QString m_arend;
public:
    RndManager();
    virtual ~RndManager();

    HRESULT getAudioRenderers(QStringList &);
    HRESULT setAudioRenderer(QString);
    QString getAudioRenderer();
};

#endif // RNDMANAGER_H
