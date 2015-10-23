#ifndef ENCMANAGER_H
#define ENCMANAGER_H

#include "graphmanager.h"
#include "moreuuids.h"
#include <wmcodecdsp.h>

class EncManager : public GraphManager
{
public:
    EncManager();
    virtual ~EncManager();

protected:
    // Encoders
    QString m_venc;
    QString m_aenc;

    // Output file
    QString m_output;

public:

    HRESULT getVideoCompressors(QStringList &);
    HRESULT getAudioCompressors(QStringList &);

    HRESULT setVideoCompressor(QString);
    HRESULT setAudioCompressor(QString);
    HRESULT setOutputFile(QString);

    QString getVideoCompressor();
    QString getAudioCompressor();
    QString getOutputFile();
};

#endif // RECMANAGER_H
