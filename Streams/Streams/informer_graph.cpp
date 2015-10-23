#include "informer_graph.h"


InformerGraph::InformerGraph()
{
}

InformerGraph::~InformerGraph()
{
    reset();
    //qDebug() << "InformerGraph::~InformerGraph";
}

void InformerGraph::reset()
{
    m_cgb.Release();
    BaseGraph::reset();
}

HRESULT InformerGraph::build()
{
    HRESULT hr = BaseGraph::build();
    if (FAILED(hr))
        return hr;

    // Create capture graph builder object
    hr = m_cgb.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "InformerGraph::build(), failed to create capture graph builder object hr=" + QString::number(hr));
        return hr;
    }

    // Set filter graph
    hr = m_cgb->SetFiltergraph(m_gb);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "InformerGraph::build(), failed to set graph builder object into capture graph builder object hr=" + QString::number(hr));
        return hr;
    }
    return hr;
}

HRESULT InformerGraph::addFilter(QString name, LPWSTR nameInGraph, CComPtr<IBaseFilter> &filter, const GUID &category)
{
    return BaseGraph::addFilter(name, nameInGraph, filter, category);
}

ICaptureGraphBuilder2 *InformerGraph::getCaptureBuilder() const
{
    return m_cgb;
}
