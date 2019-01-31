#ifndef _VUL_EXPORT_H
#define _VUL_EXPORT_H

#ifdef _WIN32
#ifdef VULPESENGINE_EXPORT
#define VEAPI __declspec(dllexport)
#else
#define VEAPI __declspec(dllimport)
#endif // VULPESENGINE_EXPORT
#else
#define VEAPI
#endif // _WIN32

#endif // _VUL_EXPORT_H
