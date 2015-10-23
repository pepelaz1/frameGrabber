#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <QObject>
#include <streams.h>
#include <atlbase.h>
//#include "logger.h"
#include "informer_graph.h"

class GraphManager : public QObject
{
    Q_OBJECT
public:
    explicit GraphManager(QObject *parent = 0);
    virtual ~GraphManager();
signals:
    
public slots:

protected:
    IGraphBuilder *m_gb;
    ICreateDevEnum *m_cde;
    IFilterMapper2 *m_fm2;

    // Logger object
    //Logger *m_logger;

    // Informer graph object
    InformerGraph *m_informer;

    HRESULT getFiltersInCategory(GUID, QStringList &);
public:
    void configure(ICreateDevEnum *, IFilterMapper2 *);
    void setFilterGraph(IGraphBuilder *);
};

#endif // GRAPH_MANAGER_H
