#ifndef SRCMANAGER_H
#define SRCMANAGER_H

#include "graphmanager.h"
#include "moreuuids.h"
#include <wmcodecdsp.h>
#include <QStringList>

class SrcManager : public GraphManager
{
public:
    SrcManager();
    virtual ~SrcManager();

    HRESULT getVideoSources(QStringList &);
    HRESULT getAudioSources(QStringList &);

    HRESULT setVideoSource(QString);
    HRESULT setAudioSource(QString);

    QString getVideoSource();
    QString getAudioSource();

    HRESULT getVideoSubtypes(QString, QStringList &);
    HRESULT getVideoResolutions(QString, QString, QStringList &);

protected:
    HRESULT prepareSource(QString, CComPtr<IAMStreamConfig> &);

    // Source devices
    QString m_vsrc;
    QString m_asrc;
};

#endif // SRCMANAGER_H
