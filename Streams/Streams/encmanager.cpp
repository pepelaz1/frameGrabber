#include "encmanager.h"
#include <QStringList>

EncManager::EncManager()
{
}

EncManager::~EncManager()
{
}

HRESULT EncManager::getVideoCompressors(QStringList &list)
{
    HRESULT hr = S_OK;
    // 1) Take filters from video compressor category an first
    getFiltersInCategory(CLSID_VideoCompressorCategory, list);

    // 2) Search for encoders in DirectShow Filters category using filter mapper
    CComPtr<IEnumMoniker> enumMon = 0;
    CComPtr<IMoniker> moniker = 0;
    ULONG f;

    GUID arrayOutTypes[6];
    arrayOutTypes[0] = MEDIATYPE_Video;
    arrayOutTypes[1] = MEDIASUBTYPE_AVC1;
    arrayOutTypes[2] = MEDIATYPE_Video;
    arrayOutTypes[3] = MEDIASUBTYPE_H264;
    arrayOutTypes[4] = MEDIATYPE_Video;
    arrayOutTypes[5] = MEDIASUBTYPE_VPP_H264;


    hr = m_fm2->EnumMatchingFilters(
            &enumMon,
            0,                  // Reserved.
            TRUE,               // Use exact match?
            MERIT_DO_NOT_USE+1, // Minimum merit.
            TRUE,               // At least one input pin?
            0,                  // Number of major type/subtype pairs for input.
            NULL,               // Array of major type/subtype pairs for input.
            NULL,               // Input medium.
            NULL,               // Input pin category.
            FALSE,              // Must be a renderer?
            TRUE,               // At least one output pin?
            3,                  // Number of major type/subtype pairs for output.
            arrayOutTypes,               // Array of major type/subtype pairs for output.
            NULL,               // Output medium.
            NULL);


    while(enumMon->Next(1, &moniker, &f) == S_OK)
    {

        CComPtr<IPropertyBag>bag;
        hr = moniker->BindToStorage(0, 0, IID_IPropertyBag,
            (void **)&bag);
        if (SUCCEEDED(hr))
        {
            CComVariant v;
            bag->Read(L"FriendlyName", &v, 0);
            CComBSTR bs;
            v.CopyTo(&bs);

            QString s((QChar*) bs.m_str, wcslen(bs.m_str));
            list.push_back(s);

            bag.Release();
        }
        moniker.Release();
    }
    return hr;
}

HRESULT EncManager::getAudioCompressors(QStringList &list)
{
    HRESULT hr = S_OK;

    // 1) Take filters from video compressor category an first
    getFiltersInCategory(CLSID_AudioCompressorCategory, list);

    // 2) Take filter from montivision audio compressor category
    getFiltersInCategory(CLSID_MontivisionAudioCompressorCategory, list);

    // 3) Search for encoders in DirectShow Filters category using filter mapper
    CComPtr<IEnumMoniker> enumMon = 0;
    CComPtr<IMoniker> moniker = 0;
    ULONG f;

    GUID arrayOutTypes[4];
    arrayOutTypes[0] = MEDIATYPE_Audio;
    arrayOutTypes[1] = MEDIASUBTYPE_RAW_AAC1;

    arrayOutTypes[2] = MEDIATYPE_Video;
    arrayOutTypes[3] = MEDIASUBTYPE_RAW_AAC1;

    hr = m_fm2->EnumMatchingFilters(
            &enumMon,
            0,                  // Reserved.
            TRUE,               // Use exact match?
            MERIT_DO_NOT_USE, // Minimum merit.
            TRUE,               // At least one input pin?
            0,                  // Number of major type/subtype pairs for input.
            NULL,               // Array of major type/subtype pairs for input.
            NULL,               // Input medium.
            NULL,               // Input pin category.
            FALSE,              // Must be a renderer?
            TRUE,               // At least one output pin?
            2,                  // Number of major type/subtype pairs for output.
            arrayOutTypes,               // Array of major type/subtype pairs for output.
            NULL,               // Output medium.
            NULL);


    while(enumMon->Next(1, &moniker, &f) == S_OK)
    {

        CComPtr<IPropertyBag>bag;
        hr = moniker->BindToStorage(0, 0, IID_IPropertyBag,
            (void **)&bag);
        if (SUCCEEDED(hr))
        {
            CComVariant v;
            bag->Read(L"FriendlyName", &v, 0);
            CComBSTR bs;
            v.CopyTo(&bs);

            QString s((QChar*) bs.m_str, wcslen(bs.m_str));
            list.push_back(s);
            //qDebug() << "s = " << s;

            bag.Release();
        }
        moniker.Release();
    }
    return hr;
}

HRESULT EncManager::setVideoCompressor(QString enc)
{
    m_venc = enc;
    return S_OK;
}

HRESULT EncManager::setAudioCompressor(QString enc)
{
    m_aenc = enc;
    return S_OK;
}

HRESULT EncManager::setOutputFile(QString output)
{
    m_output = output;
    return S_OK;
}

QString EncManager::getVideoCompressor()
{
    return m_venc;
}

QString EncManager::getAudioCompressor()
{
    return m_aenc;
}

QString EncManager::getOutputFile()
{
    return m_output;
}
