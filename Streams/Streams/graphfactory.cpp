#include "graphfactory.h"
#include "playback_graph.h"
#include "capture_graph.h"
#include "nplayback_graph.h"
#include "nrecorder_graph.h"



GraphFactory* GraphFactory::m_factory = 0;

GraphFactory::GraphFactory()
{
}

GraphFactory::~GraphFactory()
{
}

GraphFactory *GraphFactory::getInstance()
{
    if (m_factory == 0) {
        m_factory = new GraphFactory();
    }
    return m_factory;
}

void GraphFactory::resetInstance()
{
    if (m_factory)
    {
        delete m_factory;
        m_factory = 0;
    }
}

IGraph *GraphFactory::createNew(QString graphType)
{
    if (graphType == "Playback")
        return new PlaybackGraph();
    else if (graphType == "Capture")
        return new CaptureGraph();
    else if (graphType == "NPlayback")
        return new NPlaybackGraph();
    else if (graphType == "NRecorder")
        return new NRecorderGraph();

    return 0;
}
