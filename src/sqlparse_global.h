#ifndef SQLPARSE_GLOBAL_H
#define SQLPARSE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SQLPARSE_LIBRARY)
#define SQLPARSE_EXPORT Q_DECL_EXPORT
#else
#define SQLPARSE_EXPORT Q_DECL_IMPORT
#endif

#endif // SQLPARSE_GLOBAL_H
