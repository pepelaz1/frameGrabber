#ifndef INFORMERGRAPH_H
#define INFORMERGRAPH_H
#include "base_graph.h"
#include "dvdmedia.h"

class InformerGraph : public BaseGraph
{
public:
    InformerGraph();
    virtual ~InformerGraph();
    ICaptureGraphBuilder2 *getCaptureBuilder() const;

    virtual HRESULT build();
    virtual void reset();
    virtual HRESULT addFilter(QString, LPWSTR, CComPtr<IBaseFilter>&, const GUID &);
protected:
    CComPtr<ICaptureGraphBuilder2> m_cgb;
};

#endif // INFORMERGRAPH_H
