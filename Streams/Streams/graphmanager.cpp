#include "graphmanager.h"
#include <QStringList>

GraphManager::GraphManager(QObject *parent) :
    QObject(parent)
{
    m_informer = new InformerGraph();
}


GraphManager::~GraphManager()
{
    delete m_informer;
    //qDebug() << "GraphManager::~GraphManager";
}

void GraphManager::configure(ICreateDevEnum *cde, IFilterMapper2 *fm2)
{
    m_cde = cde;
    m_fm2 = fm2;
    //m_logger = Logger::GetInstance();
}

void GraphManager::setFilterGraph(IGraphBuilder *gb)
{
    m_gb = gb;
}

HRESULT GraphManager::getFiltersInCategory(GUID catId, QStringList &list)
{
   // qDebug() << "GraphManager::GetFiltersInCategory";
   HRESULT hr = S_OK;
    // Obtain a class enumerator for the audio renderers device category.
   CComPtr<IEnumMoniker> enumMon = 0;
   //qDebug() << "CreateClassEnumerator";
   //qDebug() << "catId" << catId.Data1 << catId.Data2 << catId.Data3 << catId.Data4;
   //qDebug() << "enumMon" << enumMon;
   hr = m_cde->CreateClassEnumerator(catId, &enumMon, 0);
   // qDebug() << "failed HR?";
   //qDebug() << hr;
   if (FAILED(hr) || hr != S_OK)
   {
      // qDebug() << "did it fail?";
       //m_logger->AddLogMessage(CRITICAL, "GraphManager::getFiltersInCategory(), failed to create category enumerator object hr=" + QString::number(hr));
       //qDebug() << "GraphManager::getFiltersInCategory(), failed to create category enumerator object hr=";
       return hr;
   }

   // Enumerate the monikers.
   CComPtr<IMoniker> moniker = 0;
   ULONG f;
   while(enumMon->Next(1, &moniker, &f) == S_OK)
   {
       CComPtr<IPropertyBag>bag;
       hr = moniker->BindToStorage(0, 0, IID_IPropertyBag,
           (void **)&bag);
       //qDebug() << "CHecking for succeeded HR";
       if (SUCCEEDED(hr))
       {
           //qDebug() << "HR succeeded";
           CComVariant v;
           bag->Read(L"FriendlyName", &v, 0);
           //qDebug() << "Grabbed friendly name and put in bag";
           CComBSTR bs;
           v.CopyTo(&bs);

           QString s((QChar*) bs.m_str, wcslen(bs.m_str));
           if (s.contains("DMO") == false)
             list.push_back(s);

           bag.Release();
       }
       moniker.Release();
   }
   enumMon.Release();
   return hr;
}
