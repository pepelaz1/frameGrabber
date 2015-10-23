#ifndef GRAPHFACTORY_H
#define GRAPHFACTORY_H

#include "Streams_global.h"
#include "igraph.h"
#include <QString>

class STREAMSSHARED_EXPORT GraphFactory
{
private:
    static GraphFactory* m_factory;
    GraphFactory();
    ~GraphFactory();
public:
    static GraphFactory *getInstance();
    static void resetInstance();

    IGraph *createNew(QString);
};

#endif // GRAPHFACTORY_H
