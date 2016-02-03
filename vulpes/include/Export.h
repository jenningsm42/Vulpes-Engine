#ifndef _VUL_EXPORT_H
#define _VUL_EXPORT_H

#ifdef VULPESENGINE_EXPORT
#define VEAPI __declspec(dllexport)
#else
#define VEAPI __declspec(dllimport)
#endif

#endif // _VUL_EXPORT_H