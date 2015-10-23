#ifndef TEXTOVERLAY_H
#define TEXTOVERLAY_H

#include <QObject>
#include <streams.h>
#include <atlbase.h>
#include <D3D9.h>
#include <vmr9.h>
#include "overlayitem.h"
#include "overlayfilter.h"

class TextOverlay : public QObject
{
    Q_OBJECT
public:
    explicit TextOverlay(QObject *parent = 0);
    ~TextOverlay();
   // HRESULT configure(CComPtr<IGraphBuilder>, CComPtr<IBaseFilter>, CComPtr<IBaseFilter>);
    HRESULT setGraphBuilder(CComPtr<IGraphBuilder>);
    HRESULT setRenderer(CComPtr<IBaseFilter>);
    HRESULT setEncoder(CComPtr<IBaseFilter>);
    void add(int, int, QString);
    HRESULT apply();
    void clear();

    HRESULT addFilter();
    IBaseFilter *getFilter();
signals:    
public slots:
private:
    ULONG_PTR m_gdiplusToken;

    // Graph builder object
    CComPtr<IGraphBuilder> m_gb;

    // Video Mixing Renderer
    CComPtr<IBaseFilter> m_vmr;

    // Mixer bitmap interface
    CComPtr<IVMRMixerBitmap9> m_mb9;

    // Video compressor filter
    CComPtr<IBaseFilter> m_vc;

    // Overlay filter's stuff
    OverlayFilter *m_ovl;

    //  IBasicVideo
    CComPtr<IBasicVideo> m_bv;

    // Overlay item
    QList<OverlayItem *> m_items;

    HRESULT draw();
    HRESULT insertOverlayFilter();
};

#endif // TEXTOVERLAY_H
