#include "proppagemanager.h"

PropPageManager::PropPageManager()
{
    m_categories.push_back(CLSID_VideoCompressorCategory);
    m_categories.push_back(CLSID_LegacyAmFilterCategory);
    m_categories.push_back(CLSID_AudioCompressorCategory);
    m_categories.push_back(CLSID_VideoInputDeviceCategory);
    m_categories.push_back(CLSID_AudioInputDeviceCategory);
    m_categories.push_back(CLSID_AudioRendererCategory);
}

PropPageManager::~PropPageManager()
{

}

HRESULT PropPageManager::showPropDialog(QString filterName)
{
    HRESULT hr = S_OK;
    m_informer->reset();
    hr = m_informer->build();
    if (FAILED(hr))
    {
    //m_logger->AddLogMessage(CRITICAL, "PropPageManager::showPropDialog(), failed to build informer graph hr=" + QString::number(hr));
        return hr;
    }

    bool found = false;
    CComPtr<IBaseFilter> flt;
    for ( int i = 0; i < m_categories.count(); i++)
    {
        hr = m_informer->addFilter(filterName, L"Filter", flt, m_categories[i]);
        if (SUCCEEDED(hr))
        {
             //qDebug() << "PropPageManager::showPropDialog found = true";
             found = true;
             break;
        }
    }


    if (!found)
    {
        //m_logger->AddLogMessage(CRITICAL, "PropPageManager::showPropDialog(), failed add filter to graph hr=" + QString::number(hr));
        return hr;
    }

    hr = showPropPages(flt);
    if(SUCCEEDED(hr))
        return hr;

    hr = showCompressDialog(flt);
    return hr;
}

HRESULT PropPageManager::showPropPages(CComPtr<IBaseFilter> flt)
{
    HRESULT hr = S_OK;
    CComPtr<ISpecifyPropertyPages>pp;
    hr = flt.QueryInterface(&pp);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "PropPageManager::showPropPage(), failed to query ISpecifyPropertyPages interface hr=" + QString::number(hr));
         return hr;
    }

    FILTER_INFO fi;
    flt->QueryFilterInfo(&fi);
    CComPtr<IUnknown> unk;
    flt->QueryInterface(&unk);

    CAUUID caGUID;
    pp->GetPages(&caGUID);
    pp.Release();

    OleCreatePropertyFrame(NULL, 0, 0, fi.achName,
                        1, &unk,caGUID.cElems, caGUID.pElems, 0,0, NULL);

    CoTaskMemFree(caGUID.pElems);
    unk.Release();

    return hr;
}

HRESULT PropPageManager::showCompressDialog(CComPtr<IBaseFilter> flt)
{
    HRESULT hr = S_OK;
    CComPtr<IAMVfwCompressDialogs>cd;
    hr = flt.QueryInterface(&cd);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "PropPageManager::showPropPage(), failed to query IAMVfwCompressDialogs interface hr=" + QString::number(hr));
         return hr;
    }
    hr = cd->ShowDialog(VfwCompressDialog_Config, 0);
    return hr;
}
