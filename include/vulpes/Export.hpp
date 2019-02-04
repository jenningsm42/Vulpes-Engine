#ifndef _VUL_EXPORT_HPP
#define _VUL_EXPORT_HPP

#if defined(_MSC_VER) && defined(_WINDLL)
#ifdef VULPESENGINE_EXPORT
#define VEAPI __declspec(dllexport)
#else
#define VEAPI __declspec(dllimport)
#endif // VULPESENGINE_EXPORT
#else
#define VEAPI
#endif // defined(_MSC_VER) && defined(_WINDLL)

#endif // _VUL_EXPORT_HPP
