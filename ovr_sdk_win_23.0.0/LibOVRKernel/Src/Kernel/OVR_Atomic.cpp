/************************************************************************************

Filename    :   OVR_Atomic.cpp
Content     :   Contains atomic operations and inline fastest locking
                functionality. Will contain #ifdefs for OS efficiency.
                Have non-thread-safe implementation if not available.
Created     :   September 19, 2012
Notes       :

Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

Licensed under the Oculus Master SDK License Version 1.0 (the "License");
you may not use the Oculus VR Rift SDK except in compliance with the License,
which is provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

https://developer.oculus.com/licenses/oculusmastersdk-1.0

Unless required by applicable law or agreed to in writing, the Oculus VR SDK
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_Atomic.h"
#include "OVR_Allocator.h"

#ifdef OVR_ENABLE_THREADS

// Include Windows 8-Metro compatible Synchronization API
#if defined(OVR_OS_MS) && defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
#include <synchapi.h>
#endif

namespace OVR {

// ***** Windows Lock implementation

#if defined(OVR_OS_MS)

// ***** Standard Win32 Lock implementation

// Constructors
Lock::Lock(unsigned spinCount) {
#if defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
  // On Windows 8 we use InitializeCriticalSectionEx due to Metro-Compatibility
  InitializeCriticalSectionEx(
      &cs, (DWORD)spinCount, OVR_DEBUG_SELECT(NULL, CRITICAL_SECTION_NO_DEBUG_INFO));
#else
  ::InitializeCriticalSectionAndSpinCount(
      &cs, (DWORD)spinCount); // This is available with WindowsXP+.
#endif
}

Lock::~Lock() {
  DeleteCriticalSection(&cs);
}

#endif

} // namespace OVR

#endif // OVR_ENABLE_THREADS
