/************************************************************************************

Filename    :   Logging_OutputPlugins.h
Content     :   Logging output plugins
Created     :   Oct 26, 2015
Authors     :   Chris Taylor

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

#ifndef Logging_OutputPlugins_h
#define Logging_OutputPlugins_h

#include "Logging/Logging_Library.h"

namespace ovrlog {

//-----------------------------------------------------------------------------
// Console
//
// Console window output (colorized)
// Prints at stdout level, even for errors.
// This takes about 3 milliseconds per message in debug mode.
class OutputConsole : public OutputPlugin {
 public:
  OutputConsole(bool useStdio = false);
  ~OutputConsole();

  // If true then use stdio for output instead of platform-savvy calls.
  void SetStdioUsage(bool enable);

 protected:
  virtual const char* GetUniquePluginName() override;
  virtual void Write(Level level, const char* subsystem, const char* header, const char* utf8msg)
      override;

  // If enabled then we use stdio instead of platform-specific calls. By default we
  // use direct platform calls because they are lower overhead and because (for Windows)
  // they are UTF8-savvy (unlike stdio on Windows). Defaults to false.
  bool UseStdio;
};

//-----------------------------------------------------------------------------
// DbgView
//
// This is the MSVC / DbgView log
// This takes about 150 microseconds per message in debug mode.

class OutputDbgView : public OutputPlugin {
 public:
  OutputDbgView();
  ~OutputDbgView();

 private:
  virtual const char* GetUniquePluginName() override;
  virtual void Write(Level level, const char* subsystem, const char* header, const char* utf8msg)
      override;
};

//-----------------------------------------------------------------------------
// System Application Event Log
//
// Windows Event Viewer Application Log
// This takes about 1 millisecond per message in debug mode.

class OutputEventLog : public OutputPlugin {
 public:
  OutputEventLog();
  ~OutputEventLog();

 private:
#if defined(_WIN32)
  typedef HANDLE EventSourceHandle;
#else
  typedef void* EventSourceHandle;
#endif

  // Event source handle initialized in constructor and used for logging
  EventSourceHandle hEventSource;
  Level MinReportEventLevel;

  virtual const char* GetUniquePluginName() override;
  virtual void Write(Level level, const char* subsystem, const char* header, const char* utf8msg)
      override;
};

} // namespace ovrlog

#endif // Logging_OutputPlugins_h
