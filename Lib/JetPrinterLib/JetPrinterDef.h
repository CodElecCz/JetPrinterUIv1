#pragma once

#ifdef JETPRINTERLIB_EXPORTS
#define JETPRINTER_API __declspec(dllexport)
#else
#define JETPRINTER_API __declspec(dllimport)
#endif