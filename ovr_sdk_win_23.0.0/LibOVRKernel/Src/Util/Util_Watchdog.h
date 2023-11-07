/************************************************************************************

Filename    :   Util_Watchdog.h
Content     :   Deadlock reaction
Created     :   June 27, 2013
Authors     :   Kevin Jenkins, Chris Taylor

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

*************************************************************************************/

#ifndef OVR_Util_Watchdog_h
#define OVR_Util_Watchdog_h

#include <thread>
#include "Kernel/OVR_Allocator.h"
#include "Kernel/OVR_Atomic.h"
#include "Kernel/OVR_String.h"
#include "Kernel/OVR_System.h"
#include "Kernel/OVR_Threads.h"
#include "Kernel/OVR_Timer.h"

namespace OVR {
namespace Util {

//-----------------------------------------------------------------------------
// WatchDog
//
// A watchdog is kept active ("fed") via regular calls to Feed() once per
// iteration of your thread's loop. Pass the expected duration upper bound (in
// msec) to Feed(), which will add the watchdog to the global observer if it
// hasn't been already. This value becomes the watchdog threshold-- if the next
// call to Feed() occurs after this duration has elapsed, the observer
// considers the previous loop iteration to have taken longer than expected (a
// "long cycle"). Watchdogs are removed from the global observer either via an
// explicit call to Disable() or upon destruction / going out of scope.
//
// The observer is a global object that normally does not need to be
// interacted with directly. It runs a background thread that checks on each
// watchdog every ~4s. For each watchdog, it calculates a delta from when
// Feed() was last called to the current time. If this delta exceeds the value
// passed to the last call to Feed(), then the previous loop iteration took too
// long and the observer increments a count of that watchdog's detected "long
// cycles." Long cycles are counted to protect against false positives e.g.
// when the system goes to sleep or the debugger is active. When 15 long cycles
// have been detected, the observer sees that thread as deadlocked and a report
// is generated / the offending process is terminated.

class WatchDog : public NewOverrideBase {
  friend class WatchDogObserver;

 public:
  WatchDog(const String& threadName);
  ~WatchDog();

  String GetThreadName() const;

  void Disable();
  void Enable();

  void Feed(int threshold);

 protected:
  // Use 32 bit int so assignment and comparison is atomic
  std::atomic<uint32_t> WhenLastFedMilliseconds = {0};
  std::atomic<int> ThreshholdMilliseconds = {0};

  String ThreadName;
  bool Listed;
};

//-----------------------------------------------------------------------------
// WatchDogObserver

class WatchDogObserver : public SystemSingletonBase<WatchDogObserver> {
  OVR_DECLARE_SINGLETON(WatchDogObserver);
  virtual void OnThreadDestroy() override;

  friend class WatchDog;

  std::unique_ptr<std::thread> WatchdogThreadHandle;

 public:
  // Uses the exception logger to write deadlock reports
  void EnableReporting(const String& organization = String(), const String& application = String());

  // Disables deadlock reports
  void DisableReporting();

  // This is the delay between deciding a deadlock occurred and terminating the process, to allow
  // for logging
  const int TerminationDelayMsec = 10000; // 10 seconds in msec

  // Enable/disable auto-terminate on deadlock report
  // IsDeadlocked() will return true, and after TerminationDelayMsec it will exit the process
  void SetAutoTerminateOnDeadlock(bool enable = true) {
    AutoTerminateOnDeadlock = enable;
  }

  // Is currently in a deadlock state?
  bool IsDeadlocked() const {
    return DeadlockSeen;
  }

  // Sets the callback used to trigger a Breakpad Minidump write.
  // Note: Lock does not need to be held here because only accessed on startup before deadlocks are
  // reported.
  void SetMiniDumpWriteCallback(void (*pWriteMiniDump)(void*)) {
    WriteMiniDump = pWriteMiniDump;
  }

  // Sets the callback used to add additional info to Breakpad client
  void SetAddBreakpadInfoClientCallback(
      void (*pAddBreakpadInfoClient)(const char* name, const char* value)) {
    AddBreakpadInfoClient = pAddBreakpadInfoClient;
  }

 protected:
  Lock ListLock;
  Array<WatchDog*> DogList;

  // This indicates that EnableReporting() was requested
  bool IsReporting = false;

  Event TerminationEvent;

  // Has a deadlock been seen?
  bool DeadlockSeen = false;
  bool AutoTerminateOnDeadlock = true;

  // On Windows, deadlock logs are stored in %AppData%\OrganizationName\ApplicationName\.
  // See ExceptionHandler::ReportDeadlock() for how these are used.
  String ApplicationName;
  String OrganizationName;

  void OnDeadlock(const String& deadlockedThreadName);

  // Breakpad is used to write minidump files.
  void (*WriteMiniDump)(void* pExceptionPtrs);
  void (*AddBreakpadInfoClient)(const char* name, const char* value);

 protected:
  int Run();

  void Add(WatchDog* dog);
  void Remove(WatchDog* dog);
};
} // namespace Util
} // namespace OVR

#endif // OVR_Util_Watchdog_h
