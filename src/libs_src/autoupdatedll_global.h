#ifndef AUTOUPDATEDLL_GLOBAL_H
#define AUTOUPDATEDLL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AUTOUPDATEDLL_LIBRARY)
#  define AUTOUPDATEDLLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define AUTOUPDATEDLLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AUTOUPDATEDLL_GLOBAL_H