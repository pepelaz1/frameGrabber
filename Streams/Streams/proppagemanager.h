#ifndef PROPPAGEMANAGER_H
#define PROPPAGEMANAGER_H

#include "graphmanager.h"

class PropPageManager : public GraphManager
{
private:
    QList<GUID> m_categories;

    HRESULT showPropPages(CComPtr<IBaseFilter> flt);
    HRESULT showCompressDialog(CComPtr<IBaseFilter> flt);
public:
    PropPageManager();
    virtual ~PropPageManager();

    HRESULT showPropDialog(QString);
};

#endif // PROPPAGEMANAGER_H
