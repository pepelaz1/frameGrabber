#include "base_graph.h"
#include "strsafe.h"

BaseGraph::BaseGraph(QObject *parent) :
    QObject(parent)
{
    //m_logger = Logger::GetInstance();

    // Create system device enumerator
    m_cde.CoCreateInstance(CLSID_SystemDeviceEnum);

    // Create filter mapper object
    m_fm2.CoCreateInstance(CLSID_FilterMapper2);
}

BaseGraph::~BaseGraph()
{
    m_cde.Release();
    m_fm2.Release();
}

HRESULT BaseGraph::build()
{
    HRESULT hr = S_OK;

   // qDebug() << "BaseGraph::build()";

    // Create graph builder object
    hr = m_gb.CoCreateInstance(CLSID_FilterGraph);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "BaseGraph::build(), failed to create graph builder object hr=" + QString::number(hr));
        return hr;
    }


    // Query media control interface
    hr = m_gb.QueryInterface(&m_mc);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "BaseGraph::build(), failed to query IMediaControl interface hr=" + QString::number(hr));
        return hr;
    }

   // Query media seeking interface
    hr = m_gb.QueryInterface(&m_ms);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "BaseGraph::build(), failed to query IMediaSeeking interface hr=" + QString::number(hr));
        return hr;
    }

    hr = m_gb.QueryInterface(&m_mex);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "BaseGraph::build(), failed to query IMediaEventEx interface hr=" + QString::number(hr));
        return hr;
    }

    // Add to running object table
    addToRot(m_gb,&m_rot);

    return hr;
}

void BaseGraph::reset()
{
   // qDebug() << "BaseGraph::reset";
    removeFromRot(m_rot);
    m_mc.Release();
    m_ms.Release();
    m_mex.Release();
    m_gb.Release();   
}


HRESULT BaseGraph::addToRot(IUnknown *graph, DWORD *reg)
{
    CComPtr<IMoniker> moniker = NULL;
    CComPtr<IRunningObjectTable>rot = NULL;

    if (FAILED(GetRunningObjectTable(0, &rot)))
    {
        return E_FAIL;
    }

    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
    StringCchPrintfW(wsz, STRING_LENGTH, L"FilterGraph %08x pod %08x", (DWORD_PTR)graph, GetCurrentProcessId());


    HRESULT hr = CreateItemMoniker(L"!", wsz, &moniker);
    if (SUCCEEDED(hr))
    {
        hr = rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE,
                            graph,
                            moniker,
                            reg);
    }
    return hr;
}

void BaseGraph::removeFromRot(DWORD reg)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
    {
        pROT->Revoke(reg);
        pROT->Release();
    }
}

HRESULT BaseGraph::addFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter> &filter)
{
    HRESULT hr = S_OK;
    CComPtr<IBaseFilter> flt;
    hr = flt.CoCreateInstance(guid);
    if (FAILED(hr))
        return hr;

    filter = flt;

    hr = m_gb->AddFilter(filter, name);
    if (FAILED(hr))
        return hr;

    return hr;
}


HRESULT BaseGraph::addFilter(QString name, LPWSTR nameInGraph, CComPtr<IBaseFilter> &filter, const GUID &category)
{
    HRESULT hr;
    CComPtr<IEnumMoniker> enumCat = 0;
    hr = m_cde->CreateClassEnumerator(category, &enumCat, 0);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "BaseGraph::addFilter(), failed to create category enumerator object hr=" + QString::number(hr));
        return hr;
    }

    // Enumerate the monikers.
    bool found = false;
    CComPtr<IMoniker> moniker = 0;
    ULONG f;
    while(enumCat->Next(1, &moniker, &f) == S_OK)
    {
        CComPtr<IPropertyBag>bag;
        hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&bag);
        if (SUCCEEDED(hr))
        {
            CComVariant v;
            bag->Read(L"FriendlyName", &v, 0);
            CComBSTR bs;
            v.CopyTo(&bs);

            QString s((QChar*) bs.m_str, wcslen(bs.m_str));
            if (s == name || name.contains(s))
            {
                CComPtr<IBaseFilter> flt;
                hr = moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&flt);
                if (FAILED(hr))
                {
                    // m_logger->AddLogMessage(CRITICAL, "BaseGraph::addFilter(), failed to bind moniker hr=" + QString::number(hr));
                    return hr;
                }

                filter = flt;

                hr = m_gb->AddFilter(filter, nameInGraph);
                if (FAILED(hr))
                {
                   //  m_logger->AddLogMessage(CRITICAL, "BaseGraph::addFilter(), failed to add filter to graph hr=" + QString::number(hr));
                    return hr;
                }
                found = true;
                break;
            }
            bag.Release();
        }
        moniker.Release();
    }
    if (!found)
        hr = VFW_E_NOT_FOUND;
    return hr;
}

HRESULT BaseGraph::connectFilters(CComPtr<IBaseFilter>&sflt, CComPtr<IBaseFilter>&dflt)
{
    HRESULT hr = S_OK;
    CComPtr<IEnumPins>senm;
    sflt->EnumPins(&senm);

    CComPtr<IPin>opin;
    ULONG ftch1 = 0;
    while(senm->Next(1,&opin,&ftch1) == S_OK)
    {
        PIN_DIRECTION spd;
        opin->QueryDirection(&spd);
        if (spd == PINDIR_OUTPUT)
        {
            hr = connectOutputPinToFilter(opin, dflt);
            // Return success if at least one of the output pins of downstream filter successfully connects
            if (SUCCEEDED(hr))
                return hr;
        }
        opin.Release();
    }
    return hr;
}

HRESULT BaseGraph::connectOutputPinToFilter(CComPtr<IPin>&opin, CComPtr<IBaseFilter>&dflt)
{
    CComPtr<IEnumPins>denm;
    dflt->EnumPins(&denm);

    HRESULT hr = S_OK;
    CComPtr<IPin>ipin;
    ULONG f = 0;
    while(denm->Next(1,&ipin,&f) == S_OK)
    {
        PIN_DIRECTION dpd;
        ipin->QueryDirection(&dpd);
        if (dpd == PINDIR_INPUT)
        {
            hr = m_gb->Connect(opin,ipin);
            if (SUCCEEDED(hr))
                return hr;
        }
        ipin.Release();
    }
    return hr;
}

HRESULT  BaseGraph::renderFilter(CComPtr<IBaseFilter>& filter)
{
    CComPtr<IEnumPins>enm;
    filter->EnumPins(&enm);

    HRESULT hr = S_OK;
    CComPtr<IPin>pin;
    ULONG f = 0;
    while(enm->Next(1,&pin,&f) == S_OK)
    {
        PIN_DIRECTION dp;
        pin->QueryDirection(&dp);
        if (dp == PINDIR_OUTPUT)
        {
            hr = m_gb->Render(pin);
            if (SUCCEEDED(hr))
                return hr;
        }
        pin.Release();
    }
    return hr;
}

IGraph::Graph_State BaseGraph::getState()
{
    if (!m_mc)
        return IGraph::State_Stopped;

    FILTER_STATE fs;
    m_mc->GetState(100, (OAFilterState *)&fs);
    return (IGraph::Graph_State)fs;
}
