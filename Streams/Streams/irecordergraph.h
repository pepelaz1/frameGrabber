#ifndef IRECORDERGRAPH_H
#define IRECORDERGRAPH_H

#include "Streams_global.h"
#include "igraph.h"
#include <QString>


class STREAMSSHARED_EXPORT IRecorderGraph : public IGraph
{
public:

    virtual HRESULT getVideoSources(QStringList &) = 0;
    virtual HRESULT setVideoSource(QString) = 0;
    virtual HRESULT getAudioSources(QStringList &) = 0;
    virtual HRESULT setAudioSource(QString) = 0;

    virtual HRESULT getVideoCompressors(QStringList &) = 0;
    virtual HRESULT setVideoCompressor(QString) = 0;
    virtual HRESULT getAudioCompressors(QStringList &) = 0;
    virtual HRESULT setAudioCompressor(QString) = 0;
    virtual HRESULT setOutputFile(QString) = 0;

    virtual HRESULT getVideoSubtypes(QString, QStringList &) = 0;
    virtual HRESULT getVideoResolutions(QString, QString, QStringList &) = 0;

    virtual HRESULT showPropDialog(QString) = 0;
};


#endif // ICAPTUREGRAPH_H
