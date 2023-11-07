/************************************************************************************

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

#pragma once

#include <stdint.h>

namespace ovrlog {

struct LogStringBuffer;
class Channel;
class Configurator;

typedef uint32_t Log_Level_t;
typedef uint32_t Write_Option_t;

enum class Level : Log_Level_t;

class Name;

struct LogStringBuffer;

template <typename T>
void LogStringize(LogStringBuffer& buffer, const T& thing);

class OutputPlugin;

struct ChannelNode;

class OutputWorker;

class ErrorSilencer;

class Channel;

class ConfiguratorPlugin;

class Configurator;

} // namespace ovrlog
