#include "rndmanager.h"

RndManager::RndManager()
{
}

RndManager::~RndManager()
{
}

 HRESULT RndManager::getAudioRenderers(QStringList &lst)
 {
     return getFiltersInCategory(CLSID_AudioRendererCategory, lst);
 }

 HRESULT RndManager::setAudioRenderer(QString arend)
 {
     m_arend = arend;
     return S_OK;
 }

 QString RndManager::getAudioRenderer()
 {
     return m_arend;
 }
