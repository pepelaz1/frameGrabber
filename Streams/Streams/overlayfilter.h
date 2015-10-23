#ifndef OVERLAYFILTER_H
#define OVERLAYFILTER_H
#include <InitGuid.h>
#include <streams.h>
#include <wchar.h>
#include <QList>
#include "overlayitem.h"

//class OverlayFilter : public CTransInPlaceFilter
class OverlayFilter : public CTransformFilter
{
private:
    _GUID m_subtype;
    int m_videoWidth, m_videoHeight;
    int m_bmpWidth, m_bmpHeight;
    unsigned char *m_bmpData;
    int m_textSize;
    CCritSec m_cs;
    QList<OverlayItem> m_items;
    void recreateTextBitmap();
    bool m_anyItems;

public:
    DECLARE_IUNKNOWN

    OverlayFilter(LPWSTR name, LPUNKNOWN punk, HRESULT *phr);
    virtual ~OverlayFilter();
    virtual HRESULT Transform(IMediaSample *, IMediaSample *);
    virtual HRESULT CheckInputType(const CMediaType *);
    HRESULT draw(QList<OverlayItem *> &);

    virtual HRESULT GetMediaType(int,CMediaType *);
    virtual HRESULT CheckTransform(const CMediaType *, const CMediaType *);
    virtual HRESULT DecideBufferSize(IMemAllocator *, ALLOCATOR_PROPERTIES *);
};

#endif // OVERLAYFILTER_H
