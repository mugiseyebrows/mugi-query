#ifndef EMMET_GLOBAL_H
#define EMMET_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EMMET_LIBRARY)
#define EMMET_EXPORT Q_DECL_EXPORT
#else
#define EMMET_EXPORT Q_DECL_IMPORT
#endif

#endif // EMMET_GLOBAL_H
