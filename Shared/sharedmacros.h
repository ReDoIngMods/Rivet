#pragma once

#ifndef RIVET_LIB_API
#if RIVET_LIB_EXPORTS
#define RIVET_LIB_API __declspec(dllexport)
#else
#define RIVET_LIB_API __declspec(dllimport)
#endif
#endif

#ifndef RIVET_SHARED_API
#if RIVET_SHARED_EXPORTS
#define RIVET_SHARED_API __declspec(dllexport)
#else
#define RIVET_SHARED_API __declspec(dllimport)
#endif
#endif

#ifndef RIVET_DOORSTOP_API
#if RIVET_DOORSTOP_EXPORTS
#define RIVET_DOORSTOP_API __declspec(dllexport)
#else
#define RIVET_DOORSTOP_API __declspec(dllimport)
#endif
#endif