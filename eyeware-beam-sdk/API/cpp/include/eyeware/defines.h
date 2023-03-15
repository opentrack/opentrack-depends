/**
 * Copyright and confidentiality notice
 *
 * This file is part of GazeSense SDK, which is proprietary and confidential
 * information of Eyeware Tech SA.
 *
 * Copyright (C) 2020 Eyeware Tech SA
 *
 * All rights reserved
 */

#ifndef _EYEWARE_DEFINES_
#define _EYEWARE_DEFINES_

#if !defined(EW_API_EXPORT)
#if defined(WIN32) || defined(_WIN32)
#define EW_API_EXPORT __declspec(dllexport)
#else
#define EW_API_EXPORT __attribute__((visibility("default")))
#endif
#endif

#if !defined(EW_API_IMPORT)
#if defined(EYEWARE_STATIC_LINK) || defined(EYEWARE_LINK_OBJ_LIB)
#define EW_API_IMPORT
#elif defined(WIN32) || defined(_WIN32)
#define EW_API_IMPORT __declspec(dllimport)
#else
#define EW_API_IMPORT
#endif
#endif

#if !defined(EW_API)
#if defined(EYEWARE_SDK_BUILD)
#define EW_API EW_API_EXPORT
#else
#define EW_API EW_API_IMPORT
#endif
#endif

#endif // _EYEWARE_DEFINES_
