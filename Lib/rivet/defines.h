#pragma once

#ifndef RIVET_LIB_API
#if RIVET_LIB_EXPORTS
#define RIVET_LIB_API __declspec(dllexport)
#else
#define RIVET_LIB_API __declspec(dllimport)
#endif
#endif