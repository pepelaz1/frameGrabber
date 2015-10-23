#ifndef STREAMS_GLOBAL_H
#define STREAMS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STREAMS_LIBRARY)
#  define STREAMSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define STREAMSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STREAMS_GLOBAL_H
