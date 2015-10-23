#ifndef BASE_GRAPH_H
#define BASE_GRAPH_H

#include <QObject>
#include <atlbase.h>
#include <InitGuid.h>
#include <streams.h>
//#include "logger.h"
#include "igraph.h"
//#include "moreuuids.h"
#include <wmcodecdsp.h>



class BaseGraph : public QObject
{
    Q_OBJECT
public:
    explicit BaseGraph(QObject *parent = 0);
    virtual ~BaseGraph();
signals:
    
public slots:

protected:
    DWORD m_rot;
    //Logger *m_logger;

    // Source file/url
    QString m_source;

    // Interfaces
    CComPtr<IGraphBuilder> m_gb;
    CComPtr<ICreateDevEnum> m_cde;
    CComPtr<IFilterMapper2> m_fm2;
    CComPtr<IMediaControl> m_mc;
    CComPtr<IMediaSeeking> m_ms;
    CComPtr<IMediaEventEx> m_mex;

    virtual void reset();
    virtual HRESULT build();
    HRESULT addToRot(IUnknown *graph, DWORD *reg);
    void removeFromRot(DWORD reg);

    virtual HRESULT addFilter(const GUID &, LPWSTR, CComPtr<IBaseFilter>&);
    virtual HRESULT addFilter(QString, LPWSTR, CComPtr<IBaseFilter>&, const GUID &);
    virtual HRESULT connectFilters(CComPtr<IBaseFilter>&, CComPtr<IBaseFilter>&);
    virtual HRESULT connectOutputPinToFilter(CComPtr<IPin>&, CComPtr<IBaseFilter>&);
    virtual HRESULT renderFilter(CComPtr<IBaseFilter>&);

public:
    IGraph::Graph_State getState();
};

#endif // BASE_GRAPH_H
