/************************************************************************************

Filename    :   Logging_Library.h
Content     :   Logging system
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

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4530) // C++ exception handler used, but unwind semantics are not enabled
#endif // _MSC_VER

#include <time.h>
#include <array>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#endif // _MSC_VER

#include "Logging/Logging-fwd.h"
#include "Logging/Logging_Tools.h"

#if !defined(_WIN32)
#include <chrono>
#include <condition_variable>
#include <thread>
#endif // !defined(_WIN32)

namespace ovrlog {

struct LogStringBuffer;
class Channel;
class Configurator;

typedef uint32_t Log_Level_t;
typedef uint32_t Write_Option_t;

//-----------------------------------------------------------------------------
// Log Level
//
// Log message priority is indicated by its level.  The level can inform how
// prominently it is displayed on the console window or whether or not a
// message is displayed at all.

enum class Level : Log_Level_t {
  // No logging occurs.
  Disabled,

  // Trace message.  This is a message that can potentially happen once per
  // camera/HMD frame and are probably being reviewed after they are recorded
  // since they will scroll by too fast otherwise.
  Trace,

  // Debug message.  This is a verbose log level that can be selectively
  // turned on by the user in the case of perceived problems to help
  // root-cause the problems.  This log level is intended to be used for
  // messages that happen less often than once per camera/HMD frame,
  // such that they can be human readable.
  Debug,

  // Info messages, which should be the default message type for infrequent
  // messages used during subsystem initialization and/or shutdown.  This
  // log level is fairly visible so should be used sparingly.  Expect users to
  // have these turned on, so avoid printing anything here that would obscure
  // Warning and Error messages.
  Info,

  // Warning message, which is also displayed almost everywhere.  For most
  // purposes it is as visible as an Error message, so it should also be used
  // very selectively.  The main difference from Error level is informational
  // as it is just as visible.
  Warning,

  // Highest level of logging.  If any logging is going on it will include this
  // message.  For this reason, please avoid using the Error level unless the
  // message should be displayed absolutely everywhere.
  Error,

  // Number of error levels
  Count, // Used to static assert where updates must happen in code.
};

//-----------------------------------------------------------------------------
// LOGGING_LOC
//
// C++11 Trait that can be used to insert the file and line number into a log
// message.  Often times it is a good idea to put these at the start of a log
// message so that the user can click on them to have the code editor IDE jump
// to that line of code.

// Log Line of Code FileLineInfo object.
#if defined(LOGGING_DEBUG)
#define LOGGING_FILE_LINE_STRING_ __FILE__ "(" LOGGING_STRINGIZE(__LINE__) ")"
#define LOGGING_LOC LOGGING_FILE_LINE_STRING_
#else
#define LOGGING_LOC "(no LOC)"
#endif

//-----------------------------------------------------------------------------
// LOGGING_LOCATION_HASH
//
// Provides a compile-time constexpr hash of the file/line combination, which is
// likely unique across all locations within a source code base. The primary weakness
// of this is a compiler environment in which __FILE__ represents just a file name
// and not a path, and there are two files with the same name and this is used on
// the same line within those files.
//
// Example usage:
//     printf("%llu", LOGGING_LOCATION_HASH());
//
#if defined(_WIN64) || defined(__x64__)
#define LOGGING_FILE_LINE_FNV_HASH_PRIME 1099511628211ULL
#define LOGGING_FILE_LINE_FNV_HASH_OFFSET 14695981039346656037ULL
#else
#define LOGGING_FILE_LINE_FNV_HASH_PRIME 16777619
#define LOGGING_FILE_LINE_FNV_HASH_OFFSET 2166136261
#endif

// Implementation function for calculating a hash that's ~unique for a given file/line combination.
constexpr uint64_t LoggingFileLineHash(const char* const path, uint32_t index, uint64_t hash) {
  return path[index]
      ? ovrlog::LoggingFileLineHash(
            path, index + 1, (((uint8_t)path[index] ^ hash) * LOGGING_FILE_LINE_FNV_HASH_PRIME))
      : hash;
}

#ifdef _MSC_VER
#define LOGGING_LOCATION_HASH()                                                               \
  __pragma(warning(push)) __pragma(warning(disable : 4307)) ovrlog::LoggingFileLineHash(      \
      __FILE__,                                                                               \
      0,                                                                                      \
      (uint64_t)(                                                                             \
          (__LINE__ ^ LOGGING_FILE_LINE_FNV_HASH_OFFSET) * LOGGING_FILE_LINE_FNV_HASH_PRIME)) \
      __pragma(warning(pop))
#else
#define LOGGING_LOCATION_HASH() \
  ovrlog::LoggingFileLineHash(  \
      __FILE__,                 \
      0,                        \
      (uint64_t)(               \
          (__LINE__ ^ LOGGING_FILE_LINE_FNV_HASH_OFFSET) * LOGGING_FILE_LINE_FNV_HASH_PRIME))
#endif

//-----------------------------------------------------------------------------
// LogTime
//
// Our time type
//
#if defined(WIN32)
typedef SYSTEMTIME LogTime;
#else
typedef std::chrono::system_clock::time_point LogTime;
#endif

// Gets the current time in LogTime format.
// May be called from multiple threads concurrently.
LogTime GetCurrentLogTime();

//-----------------------------------------------------------------------------
// Name
//
// A fixed length name which avoids allocation and is safe to share across DLL
// boundaries.

class Name {
 public:
  // Longest string not including '\0' termination
  static const size_t MaxLength = 63;

  Name(const char* init) {
    // Maximum portability vs. ::strncpy_s
    for (size_t i = 0; i < MaxLength; ++i) {
      if ((name[i] = init[i]) == '\0')
        return;
    }
    name[MaxLength] = '\0';
  }
  Name(std::string init) : Name(init.c_str()) {}
  const char* Get() const {
    return name.data();
  }
  int cmp(const Name& rhs) {
    // Maximum portability vs. std::strncmp
    int diff = 0;
    for (size_t i = 0; i < MaxLength; ++i) {
      // this < rhs => (-), this > rhs => (+)
      diff = int(name[i]) - int(rhs.name[i]);
      // If strings are equal it is sufficent to terminate on either '\0'
      if ((diff != 0) || (name[i] == '\0')) // => (rhs.name[i] == '\0')
        break;
    }
    return diff;
  }
  bool operator==(const Name& rhs) {
    return cmp(rhs) == 0;
  }
  bool operator!=(const Name& rhs) {
    return cmp(rhs) != 0;
  }

 private:
  // '\0'-terminated
  std::array<char, MaxLength + 1> name;
};

//-----------------------------------------------------------------------------
// LogStringBuffer
//
// Thread-local buffer for constructing a log message.

struct LogStringBuffer {
  const Name SubsystemName;
  const Level MessageLogLevel;

  // Buffer containing string as it is constructed
  std::stringstream Stream;
  // TBD: We can optimize this better than std::string
  // TBD: We can remember the last log string size to avoid extra allocations.

  // Flag indicating that the message is being relogged.
  // This is useful to prevent double-logging messages.
  bool Relogged;

  // Ctor
  LogStringBuffer(const char* subsystem, Level level)
      : SubsystemName(subsystem), MessageLogLevel(level), Stream(), Relogged(false) {}
};

//-----------------------------------------------------------------------------
// LogStringize Override
//
// This is the function that user code can override to control how special types
// are serialized into the log messages.

// Delete logging a shared_ptr
template <typename T>
LOGGING_INLINE void LogStringize(LogStringBuffer& buffer, const std::shared_ptr<T>& thing) {
  (void)buffer;
  (void)thing;
#if !defined(__clang__)
  static_assert(
      false, "Don't log a shared_ptr, log *ptr (or ptr.get() for the raw pointer value).");
#endif
}

// Delete logging a unique_ptr
template <typename T, typename Deleter>
LOGGING_INLINE void LogStringize(
    LogStringBuffer& buffer,
    const std::unique_ptr<T, Deleter>& thing) {
  (void)buffer;
  (void)thing;
#if !defined(__clang__)
  static_assert(
      false, "Don't log a unique_ptr, log *ptr (or ptr.get() for the raw pointer value).");
#endif
}

template <typename T>
LOGGING_INLINE void LogStringize(LogStringBuffer& buffer, const T& first) {
  buffer.Stream << first;
}

// Overrides for various types we want to handle specially:

template <>
LOGGING_INLINE void LogStringize(LogStringBuffer& buffer, const bool& first) {
  buffer.Stream << (first ? "true" : "false");
}

template <>
void LogStringize(LogStringBuffer& buffer, wchar_t const* const& first);

template <int N>
LOGGING_INLINE void LogStringize(LogStringBuffer& buffer, const wchar_t (&first)[N]) {
  const wchar_t* str = first;
  LogStringize(buffer, str);
}

template <>
LOGGING_INLINE void LogStringize(LogStringBuffer& buffer, const std::wstring& first) {
  const wchar_t* str = first.c_str();
  LogStringize(buffer, str);
}

//-----------------------------------------------------------------------------
// Log Output Worker Thread
//
// Worker thread that produces the output.
// Call AddPlugin() to register an output plugin.

// User-defined output plugin
class OutputPlugin {
 public:
  virtual ~OutputPlugin() {}

  // Return a unique string naming this output plugin.
  virtual const char* GetUniquePluginName() = 0;

  // Write data to output.
  virtual void
  Write(Level level, const char* subsystem, const char* header, const char* utf8msg) = 0;
};

//-----------------------------------------------------------------------------
// Used by channel to specify how to write
enum class WriteOption : Write_Option_t {
  // Default log write. If the output message queue size limit is reached, then the write
  // is discarded. This is intended to be an unusual case that occurs only if the message
  // generation is very rapid compared to queue processing.
  Default,

  // If the Write would require adding an entry that goes over the max queue size, usually we
  // drop the write. But this flag indicates to exceed the queue size, which can user more memory
  // than expected and possibly exhaust memory if it is used very much.
  DangerouslyIgnoreQueueLimit
};

//--------------------------------------------------------------------------------------------------
// RepeatedMessageManager
//
// We have a problem in which sometimes a subsystem will spam the log with essentially the
// same message repeatedly.
//
// This class handles the ownership of repeated messages, which it aggregates and counts.
// The logging system can use an instance of this class to aggregate redundant messages.
//
// Example usage:
//    RepeatedMessageManager gRepeatedMessageManager; // Some kind of singleton.
//
//    void OutputWorker::Write(const char* subsystemName, Level messageLogLevel,
//                              const char* stream, bool relogged, WriteOption option)
//    {
//        if (gRepeatedMessageManager.HandleMessage(subsystemName, messageLogLevel, stream) ==
//        HandleMessage::Aggregated)
//            return;
//    }
//
//    void OutputWorker::ProcessQueuedMessages() {
//        while(...) {
//            [...]
//            gRepeatedMessageManager.Poll();
//        }
//    }
//
// Design
//    In our HandleMessage function:
//    For every message we receive, we look at the last N messages received and see if this message
//    appears to be a duplicate of any of the previous ones. We make that decision by first checking
//    if the text of the message matches an entry in a hash table of known currently repeating
//    messages. If not present then we make that decision based on the text of the message being
//    similar to message text of the last N received messages and being received within some period
//    of time since the last similar message. It it appears to be a repeat, then we add it to the
//    hash table.
//
//    In our Poll function:
//    Each message in the hash table has a repeat count, which is the number of times
//    the message has been previously seen since it was last printed. When that count gets above
//    some value, we print the message and set the count to zero for further accumulation.
//    Messages that have been in the hash table without having been repeated recently are printed
//    a final time and removed from the hash table. Since this final printing occurs only after a
//    timeout, it will be occurring after the last message was received and thus be delayed by some
//    amount of time.
//
//    Performance considerations:
//    The list of N previously received messages represents the biggest potential performance cost
//    to this functionality. We need to check the currently incoming message against each of the
//    last N messages. That check needs to be fast, and the maintenance of the data structure
//    which holds that data (e.g. container heap usage) needs to be fast. One fairly fast solution
//    is to maintain the last N messages as a hash table of string hashes, and so when a new message
//    is received, we do an O(1) hash table lookup. But we need to continuously prune the hash
//    table, which is an O(n) operation unless we have some kind of priority_queue alongside the
//    hash table which tells us how to quickly find the oldest entries in the hash table. A simpler
//    solution is to prune the hash table only when it gets beyond N*2 messages. So we only do
//    this O(n) pass periodically.
//
class RepeatedMessageManager {
 public:
  RepeatedMessageManager();
  ~RepeatedMessageManager() = default;

  enum class HandleResult {
    Aggregated, // The message was consumed for aggregation (it was a repeat). Don't print it.
    Passed // The message didn't appear to be a repeat and should be printed.
  };

  // If the message appears to be a repeat of a recent message then we add it to our repeated
  // message database and return HandleResult::Aggregated. If HandleMessage returns Aggreggated
  // then the caller should not write the message to the stream.
  HandleResult HandleMessage(const char* subsystemName, Level messageLogLevel, const char* stream);

  // This function should be called at least once every maxDeferrableDetectionTimeMs.
  // It causes us to look at the list of repeated messages and possibly print an aggregated
  // form of any of them that has expired. For example, if the message "hello world" was repeated
  // 100 times in the last five seconds, we print a single aggregate message such as:
  // "[repeated 100 times] hello world". We also do housekeeping on the recent message list.
  // The outputWorker is where any deferred aggregated messages should be written.
  void Poll(OutputWorker* outputWorker);

  // Causes messages with the given prefix to not be subject to aggregated deferral.
  // This is useful in case you want a specific message to be repeated, regardless of the
  // frequency or apparent redundancy it may have. The MessagePrefix needs to be at least
  // messagePrefixLength in length, so that it can be matched internally accurately.
  // Strings are case-sensitive.
  void AddRepeatedMessageException(const char* messagePrefix);

  // Removes messages previously added with AddRepeatedMessageException.
  // Strings are case-sensitive.
  void RemoveRepeatedMessageException(const char* messagePrefix);

  // Causes messages with the given subsystemName to not be subject to aggregated deferral.
  // This is useful in case you want a specific subsystem's message to be repeated,
  // regardless of the frequency or apparent redundancy it may have.
  // Strings are case-sensitive.
  void AddRepeatedMessageSubsystemException(const char* subsystemName);

  // Removes messages previously added with AddRepeatedMessageSubsystemException.
  void RemoveRepeatedMessageSubsytemException(const char* subsystemName);

 protected:
  // Keep the last <recentMessageCount> messages to see if any of them are repeated.
  static const uint32_t recentMessageCount = 40;

  // The number of leading characters in a message which we consider for comparisons.
  static const uint32_t messagePrefixLength = 36;

  // If a message is a repeat of a previous message, we still print it in the log a few times
  // before we start silencing it and holding it for later aggregation.
  static const uint32_t printedRepeatCount = 8;

  //  Max number of messages that are held for deferral before we print an aggregate message.
  static const uint32_t maxDeferredMessages = 40;

  // If a repeating message wasn't encountered in the last <maxDeferrableDetectionTimeMs> time,
  // then we don't consider it repeating any more.
  static const uint32_t maxDeferrableDetectionTimeMs = 1000;

  // Every <purgeDeferredMessageTimeMs> time period, we do a sweep of the repeated message map to
  // prune any old entries that don't look like they are repeating any more.
  static const uint32_t purgeDeferredMessageTimeMs = 100;

  // String hash used to identify similar messages.
  typedef uint32_t PrefixHashType;

  // For our uses we don't want LogTime, which is a calendar time that's hard and slow to work
  // with. Instead we want to compare milliseconds in an absolute way. So we define a type that
  // is absolute milliseconds which can derived from a LogTime.
  typedef int64_t LogTimeMs;

  // Stores a recently generated message. It doesn't store the entire message string,
  // as we care only about the first N characters of messages. We store the last M messages
  // ever received in a list of RecentMessages. That way we can tell if there was a recent repeat.
  // To consider: we don't currently consider the subsystem name as part of the uniqueness of the
  // string. We may want or need to do this, but it's very unlikely two strings would be the same
  // while coming from separate subsystems.
  struct RecentMessage {
    LogTimeMs timeMs; // Time that the message was generated.
                      // Don't need to store the message or message prefix itself, though it may
                      // help debugging.
  };
  typedef std::unordered_map<PrefixHashType, RecentMessage> RecentMessageMapType;

  // Represents a message which has been identified as being repeated. This struct allows us to
  // know how many times the message was repeated, when it was first seen, etc.
  struct RepeatedMessage {
    std::string subsystemName; // To consider: Convert to char [16]
    Level messageLogLevel; // log level, e.g. Level::Trace.
    std::string stream; // The first message of the repeated set.
    LogTimeMs initialTimeMs; // Time the message was first seen.
    LogTimeMs lastTimeMs; // Time the most recent repeat of the message was seen.
    uint32_t aggregatedCount; // Number of times this message was aggregated for later.
    uint32_t printedCount; // Number of times this message has been 'printed';
                           //  aggregate printing counts multiple times towards this.
    RepeatedMessage()
        : subsystemName(),
          messageLogLevel(),
          stream(),
          initialTimeMs(),
          lastTimeMs(),
          aggregatedCount(),
          printedCount() {}
    RepeatedMessage(
        const char* subsystemName_,
        Level messageLogLevel_,
        const char* stream_,
        LogTimeMs initialTimeMs_,
        LogTimeMs lastTimeMs_,
        uint32_t aggregatedCount_)
        : subsystemName(subsystemName_),
          messageLogLevel(messageLogLevel_),
          stream(stream_),
          initialTimeMs(initialTimeMs_),
          lastTimeMs(lastTimeMs_),
          aggregatedCount(aggregatedCount_),
          printedCount() {}
  };
  typedef std::unordered_map<PrefixHashType, RepeatedMessage> RepeatedMessageMapType;

  // Prints a message that's an aggregate deferred printing.
  void PrintDeferredAggregateMessage(OutputWorker* outputWorker, RepeatedMessage& repeatedMessage);

  // Calculates a hash for the given string for at most <messagePrefixLength> characters.
  static PrefixHashType GetHash(const char* str);

  // Gets the current LogTime in LogTimeMs.
  static LogTimeMs GetCurrentLogMillisecondTime();

  // Converts LogTime to LogTimeMs.
  static LogTimeMs LogTimeToMillisecondTime(const LogTime& logTime);

  // Returns end - begin, possibly with some fixup.
  static int64_t GetLogMillisecondTimeDifference(LogTimeMs begin, LogTimeMs end);

 protected:
  // Mutex to which covers all our member data.
  std::recursive_mutex Mutex;

  // Used to protect against re-entrancy. Our Poll function calls external code, and we want to
  // prevent there being a problem if that external code unexpectedly calls us back.
  bool BusyInWrite;

  // To do: Get a fixed-size node allocator working with this container for low overhead.
  RecentMessageMapType RecentMessageMap;

  // To consider: Get a fixed-size node allocator working with this container for low overhead.
  RepeatedMessageMapType RepeatedMessageMap;

  // We don't need to store the string, just the string hash.
  std::unordered_set<PrefixHashType> RepeatedMessageExceptionSet;

  // We don't need to store the string, just the string hash.
  std::unordered_set<PrefixHashType> RepeatedMessageSubsystemExceptionSet;
};

// Iterate through the list of channels before the CRT has initialized
#pragma pack(push, 1)
struct ChannelNode {
  const char* SubsystemName; // This is always a pointer to a Channel's SubsystemName.Get()
  Log_Level_t* Level;
  bool* UserOverrodeMinimumOutputLevel;
  ChannelNode* Next;
};
#pragma pack(pop)

// Export the function to access OutputWorker::Write(). This is used by the Channel class
// to allow writing with OutputWorker possibly in a separate module.
void OutputWorkerOutputFunctionC(
    const char* subsystemName,
    Log_Level_t messageLogLevel,
    const char* stream,
    bool relogged,
    Write_Option_t option);

typedef void (*OutputWorkerOutputFunctionType)(
    const char* subsystemName,
    Log_Level_t messageLogLevel,
    const char* stream,
    bool relogged,
    Write_Option_t option);

// Shutdown the logging system and release memory
void ShutdownLogging();

// Restart the logging system
void RestartLogging();

// Log Output Worker Thread
class OutputWorker {
  OutputWorker(); // Use GetInstance() to get the singleton instance.

 public:
  static OutputWorker* GetInstance();

  ~OutputWorker();

  void InstallDefaultOutputPlugins();

  // Start/stop logging output (started automatically)
  void Start();
  void Stop();

  // Blocks until all log messages before this function call are completed.
  void Flush();

  // Write a log buffer to the output
  // relogged indicates that the message is being relogged, which is useful to prevent
  // double-logging messages.

  void Write(
      const char* subsystemName,
      Level messageLogLevel,
      const char* stream,
      bool relogged,
      WriteOption option);

  // Plugin management
  void AddPlugin(std::shared_ptr<OutputPlugin> plugin);
  void RemovePlugin(std::shared_ptr<OutputPlugin> plugin);
  std::shared_ptr<OutputPlugin> GetPlugin(const char* const pluginName);

  // Disable all output
  void DisableAllPlugins();

  // Get the lock used for the channels.
  Lock* GetChannelsLock();

  // Add an exception to the RepeatedMessageManager
  void AddRepeatedMessageSubsystemException(const char* subsystemName);

  // Removes messages previously added with AddRepeatedMessageException
  void RemoveRepeatedMessageSubsystemException(const char* subsystemName);

  // Sets the set of channels that can write to the given output.
  // By default, all outputs are written to by all channels. But if you provide a set of
  // channels with this function, then the output is written to only by the given channels.
  // A typical use case of this is to associate a given channel 1:1 a given output,
  // by combining usage of this function with the SetChannelOutputPlugins function.
  //
  // Example usage, which sets a 1:1 correspondence between a TimingData channel and a
  // single log file for its output:
  //     ow->SetOutputPluginChannels("TimingDataLog", {"TimingData"});
  //     ow->SetChannelOutputPlugins("TimingData", {"TimingDataLog"});
  // or alternatively:
  //     ow->SetChannelSingleOutput("TimingData", "TimingDataLog");
  //
  void SetOutputPluginChannels(
      const char* outputPluginName,
      const std::vector<std::string>& channelNames);

  // Sets the set of outputs that a given channel can write to.
  void SetChannelOutputPlugins(
      const char* channelName,
      const std::vector<std::string>& outputPluginNames);

  // This combines usage of SetOutputPluginChannels and SetChannelOutputPlugins to associate a
  // channel to exclusively the given output. No other channels will write to the given output,
  // and the channel will write to no other outputs.
  void SetChannelSingleOutput(const char* channelName, const char* outputPluginName);

 private:
  // Is the logger running in a debugger?
  bool IsInDebugger;

  bool DefaultPluginsDisabled;

  // It's here so we know it is valid in the scope of ~OutputWorker
  Lock ChannelsLock;

  // Plugins
  Lock PluginsLock;
  std::set<std::shared_ptr<OutputPlugin>> Plugins;

  // Output -> channel filtering
  // This is a case-sensitive map of output name to a set of channel names.
  // Normally a given channel write is sent to all output plugins. But we support being explicit
  // about the output than that. Behavior:
  //     If an output isn't present in this map then its writable by all channels.
  //     If an output is present, then it is writable only by the channels specified by the map.
  // Threaded access to this is protected by PluginsLock.
  std::unordered_map<std::string, std::unordered_set<std::string>> OutputPluginChannelFiltering;

  // Channel -> output filtering
  // This is a case-sensitive map of channel name to a set of output plugins.
  // Normally a given channel write is sent to all output plugins. But we support being explicit
  // about the output than that. Behavior:
  //     If a channel isn't present in this map then the channel is written to all outputs.
  //     If a channel is present, then it is written only to the outputs specified by the map.
  // Threaded access to this is protected by PluginsLock.
  std::unordered_map<std::string, std::unordered_set<std::string>> ChannelOutputPluginFiltering;

  // Worker Log Buffer
  struct QueuedLogMessage {
    const Name SubsystemName;
    const Level MessageLogLevel;
    std::string Buffer;
    LogTime Time;
    QueuedLogMessage* Next;
#if defined(_WIN32)
    OvrLogHandle FlushEvent;
#else
    // Is this a fake event, just for the purpose of flushing the queue?
    bool FlushEvent;
#endif // defined(_WIN32)

    QueuedLogMessage(
        const char* subsystemName,
        Level messageLogLevel,
        const char* stream,
        const LogTime& time);
  };

  // Maximum number of logs that we allow in the queue at a time.
  // If we go beyond this limit, we keep a count of additional logs that were lost.
  static const int WorkQueueLimit = 1000;

  Lock WorkQueueLock; // Lock guarding the work queue
  QueuedLogMessage* WorkQueueHead; // Head of linked list of work that is queued
  QueuedLogMessage* WorkQueueTail; // Tail of linked list of work that is queued
  int WorkQueueSize; // Size of the linked list of queued work
  int WorkQueueOverrun; // Number of log messages that exceeded the limit
  // The work queue size is used to avoid overwhelming the logging thread, since it takes 1-2
  // milliseconds to log out each message it can easily fall behind a large amount of logs.  Lost
  // log messages are added to the WorkQueueOverrun count so that they can be reported as "X logs
  // were lost".

  inline void WorkQueueAdd(QueuedLogMessage* msg) {
    if (WorkQueueTail) {
      WorkQueueTail->Next = msg;
    } else {
      WorkQueueHead = msg;
    }
    WorkQueueTail = msg;
    ++WorkQueueSize;
  }

#if defined(_WIN32)
#define OVR_THREAD_FUNCTION_TYPE DWORD WINAPI
#else
#define OVR_THREAD_FUNCTION_TYPE uint32_t
#endif

  static OVR_THREAD_FUNCTION_TYPE WorkerThreadEntrypoint_(void* worker);

  void WorkerThreadEntrypoint();

  Lock StartStopLock;
#if defined(_WIN32)
  // Event letting the worker thread know the queue is not empty
  AutoHandle WorkerWakeEvent;
  Terminator WorkerTerminator;
  AutoHandle LoggingThread;
#else
  std::atomic<bool> Terminated;
  std::mutex WorkerCvMutex;
  std::condition_variable WorkerCv;
  std::thread LoggingThread;
#endif // defined(_WIN32)
  RepeatedMessageManager RepeatedMessageManagerInstance;

  // Append level and subsystem name to timestamp buffer
  // The buffer should point to the ending null terminator of
  // the timestamp string.
  static void
  AppendHeader(char* buffer, size_t bufferBytes, Level level, const char* subsystemName);

  void ProcessQueuedMessages();

  void
  FlushDbgViewLogImmediately(const char* subsystemName, Level messageLogLevel, const char* stream);
};

//-----------------------------------------------------------------------------
// ErrorSilencer
//
// This will demote errors to warnings in the log until it goes out of scope.
// Helper class that allows error silencing to be done several function calls
// up the stack and checked down the stack.
class ErrorSilencer {
 public:
  // Returns a bitfield of SilenceOptions that are currently in effect
  static int GetSilenceOptions();

  // Start silencing errors.
  ErrorSilencer(int options = DemoteErrorsToWarnings);

  enum SilenceOptions {
    // Error logs will be demoted to the warning log level
    DemoteErrorsToWarnings = 1,

    // All Log* methods will be silenced
    CompletelySilenceLogs = 2,

    // OVR::MakeError will not assert when errors are set
    PreventErrorAsserts = 4,

    // All logs at a level > Debug will be set to Debug level
    DemoteToDebug = 8
  };

  // Stop silencing errors.
  ~ErrorSilencer();

 private:
  // Start silencing errors.  This is done automatically be the constructor.
  void Silence();

  // Stop silencing errors.  This is done automatically be the deconstructor.
  void Unsilence();

  int Options = 0;
};

//-----------------------------------------------------------------------------
// Channel
//
// One named logging channel.

class Channel {
 public:
  Channel(const char* nameString);
  Channel(const Channel& other);
  ~Channel();

  // This is the initial default minimum level for a newly constructed Channel.
  const Level DefaultMinimumOutputLevel = Level::Info;

  // Same thing as channel name.
  // To do: rename all usage of SubsystemName to ChannelName.
  const Name SubsystemName;

  // Deprecated, use SubsystemName.Get() instead
  const char* GetName() const {
    return SubsystemName.Get();
  }

  // Add an extra prefix to all log messages generated by the channel.
  // This function is *not* thread-safe.  Logging from another thread while changing
  // the prefix can cause crashes.
  std::string GetPrefix() const;
  void SetPrefix(const std::string& prefix);

  // Set the minimum output level permitted from this channel.
  void SetMinimumOutputLevel(Level newLevel);

  // Set the output level temporarily for this session without remembering that setting.
  void SetMinimumOutputLevelNoSave(Level newLevel);

  Level GetMinimumOutputLevel() const;

  LOGGING_INLINE bool Active(Level level) const {
    return MinimumOutputLevel <= (uint32_t)level;
  }

  // Target of doLog function
  static void SetOutputWorkerOutputFunction(OutputWorkerOutputFunctionType function) {
    OutputWorkerOutputFunction = function;
  }

  template <typename... Args>
  LOGGING_INLINE void Log(Level level, Args&&... args) const {
    if (Active(level)) {
      doLog(level, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogError(Args&&... args) const {
    if (Active(Level::Error)) {
      doLog(Level::Error, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogWarning(Args&&... args) const {
    if (Active(Level::Warning)) {
      doLog(Level::Warning, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogInfo(Args&&... args) const {
    if (Active(Level::Info)) {
      doLog(Level::Info, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogDebug(Args&&... args) const {
    if (Active(Level::Debug)) {
      doLog(Level::Debug, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogTrace(Args&&... args) const {
    if (Active(Level::Trace)) {
      doLog(Level::Trace, std::forward<Args>(args)...);
    }
  }

  // printf style log functions
  template <typename... Args>
  LOGGING_INLINE void LogF(Level level, Args&&... args) const {
    if (Active(level)) {
      doLogF(level, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogErrorF(Args&&... args) const {
    if (Active(Level::Error)) {
      doLogF(Level::Error, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogWarningF(Args&&... args) const {
    if (Active(Level::Warning)) {
      doLogF(Level::Warning, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogInfoF(Args&&... args) const {
    if (Active(Level::Info)) {
      doLogF(Level::Info, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogDebugF(Args&&... args) const {
    if (Active(Level::Debug)) {
      doLogF(Level::Debug, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  LOGGING_INLINE void LogTraceF(Args&&... args) const {
    if (Active(Level::Trace)) {
      doLogF(Level::Trace, std::forward<Args>(args)...);
    }
  }

  // DANGER DANGER DANGER
  // This function forces a log message to be recorded even if the log queue is full.
  // This is dangerous because the caller can run far ahead of the output writer thread
  // and cause a large amount of memory to be allocated and logging tasks can take many
  // minutes to flush afterwards.  It should only be used when the data is critical.
  template <typename... Args>
  LOGGING_INLINE void DangerousForceLog(Level level, Args&&... args) const {
    if (Active(level)) {
      int silenceOptions = ErrorSilencer::GetSilenceOptions();
      if (silenceOptions & ErrorSilencer::CompletelySilenceLogs) {
        return;
      }

      if (level > Level::Debug && (silenceOptions & ErrorSilencer::DemoteToDebug)) {
        // Demote to debug
        level = Level::Debug;
      } else if (
          level == Level::Error && (silenceOptions & ErrorSilencer::DemoteErrorsToWarnings)) {
        // Demote to warning
        level = Level::Warning;
      }

      LogStringBuffer buffer(SubsystemName.Get(), level);

      writeLogBuffer(buffer, Prefix, args...);

      // Submit buffer to logging subsystem
      const std::string& tmp = buffer.Stream.str();
      OutputWorkerOutputFunction(
          buffer.SubsystemName.Get(),
          (Log_Level_t)buffer.MessageLogLevel,
          tmp.c_str(),
          buffer.Relogged,
          (Write_Option_t)WriteOption::DangerouslyIgnoreQueueLimit);
    }
  }
  // DANGER DANGER DANGER

 private:
  //-------------------------------------------------------------------------
  // Internal Implementation

  Channel() = delete;
  Channel(Channel&& other) = delete;
  Channel& operator=(const Channel& other) = delete;
  Channel& operator=(Channel&& other) = delete;

  friend class Configurator;

  // Used to iterate through a linked list of Channel objects
  // A linked list is used to avoid CRT new / delete during startup as this is called from the
  // constructor
  ChannelNode Node;
  void registerNode();

  // Level at which this channel will log.
  Log_Level_t MinimumOutputLevel;

  // Optional prefix
  std::string Prefix;

  // So changing Prefix is threadsafe
  mutable Lock PrefixLock;

  bool UserOverrodeMinimumOutputLevel;

  // Target of doLog function
  static OutputWorkerOutputFunctionType OutputWorkerOutputFunction;

  // Target of OnChannelLevelChange
  static void ConfiguratorOnChannelLevelChange(
      const char* channelName,
      Log_Level_t minimumOutputLevel);

  // Target of Register
  static void ConfiguratorRegister(ChannelNode* channelNode);

  // Target of Unregister
  static void ConfiguratorUnregister(ChannelNode* channelNode);

  template <typename T>
  LOGGING_INLINE void writeLogBuffer(LogStringBuffer& buffer, T&& arg) const {
    LogStringize(buffer, arg);
  }

  template <typename T, typename... Args>
  LOGGING_INLINE void writeLogBuffer(LogStringBuffer& buffer, T&& arg, Args&&... args) const {
    writeLogBuffer(buffer, arg);
    writeLogBuffer(buffer, args...);
  }

  // Unroll arguments
  template <typename... Args>
  LOGGING_INLINE void doLog(Level level, Args&&... args) const {
    int silenceOptions = ErrorSilencer::GetSilenceOptions();
    if (silenceOptions & ErrorSilencer::CompletelySilenceLogs) {
      return;
    }

    if (level > Level::Debug && (silenceOptions & ErrorSilencer::DemoteToDebug)) {
      // Demote to debug
      level = Level::Debug;
    } else if (level == Level::Error && (silenceOptions & ErrorSilencer::DemoteErrorsToWarnings)) {
      // Demote to warning
      level = Level::Warning;
    }

    LogStringBuffer buffer(SubsystemName.Get(), level);

    writeLogBuffer(buffer, Prefix, args...);

    // Submit buffer to logging subsystem
    const std::string& tmp = buffer.Stream.str();
    OutputWorkerOutputFunction(
        buffer.SubsystemName.Get(),
        (Log_Level_t)buffer.MessageLogLevel,
        tmp.c_str(),
        buffer.Relogged,
        (Write_Option_t)WriteOption::Default);
  }

  // Returns the buffer capacity required to printf the given format+arguments.
  // Returns -1 if the format is invalid.
  static int GetPrintfLengthV(const char* format, va_list argList) {
    int size;

#if defined( \
    _MSC_VER) // Microsoft doesn't support C99-Standard vsnprintf, so need to use _vscprintf.
    size = _vscprintf(format, argList); // Returns the required strlen, or -1 if format error.
#else
    size = vsnprintf(
        nullptr, 0, format, argList); // Returns the required strlen, or negative if format error.
#endif

    if (size > 0) // If we can 0-terminate the output...
      ++size; // Add one to account for terminating null.
    else
      size = -1;

    return size;
  }

  static int GetPrintfLength(const char* format, ...);

  template <typename... Args>
  LOGGING_INLINE void doLogF(Level level, Args&&... args) const {
    int silenceOptions = ErrorSilencer::GetSilenceOptions();
    if (silenceOptions & ErrorSilencer::CompletelySilenceLogs) {
      return;
    }

    if (level > Level::Debug && (silenceOptions & ErrorSilencer::DemoteToDebug)) {
      // Demote to debug
      level = Level::Debug;
    } else if (level == Level::Error && (silenceOptions & ErrorSilencer::DemoteErrorsToWarnings)) {
      // Demote to warning
      level = Level::Warning;
    }

    LogStringBuffer buffer(SubsystemName.Get(), level);

    char logCharsLocal[1024];
    char* logChars = logCharsLocal;
    char* logCharsAllocated = nullptr;

#if defined(_MSC_VER)
    int result = _snprintf_s(logCharsLocal, sizeof(logCharsLocal), _TRUNCATE, args...);
#else
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#pragma clang diagnostic ignored "-Wformat-security"
#endif // defined(__clang__)
    int result = snprintf(logCharsLocal, sizeof(logCharsLocal), args...);

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)
#endif

    if ((result < 0) || ((size_t)result >= sizeof(logCharsLocal))) {
      int requiredSize = GetPrintfLength(args...);

      if ((requiredSize < 0) || (requiredSize > (1024 * 1024))) {
        LOGGING_DEBUG_BREAK(); // This call should be converted to the new log system.
        return;
      }

      logCharsAllocated = new char[requiredSize];
      logChars = logCharsAllocated;

#if defined(_MSC_VER)
      _snprintf_s(logChars, (size_t)requiredSize, _TRUNCATE, args...);
#else
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#pragma clang diagnostic ignored "-Wformat-security"
#endif // defined(__clang__)
      snprintf(logChars, (size_t)requiredSize, args...);

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)
#endif
    }

    writeLogBuffer(buffer, Prefix, logChars);

    // Submit buffer to logging subsystem
    const std::string& tmp = buffer.Stream.str();
    OutputWorkerOutputFunction(
        buffer.SubsystemName.Get(),
        (Log_Level_t)buffer.MessageLogLevel,
        tmp.c_str(),
        buffer.Relogged,
        (Write_Option_t)WriteOption::Default);

    delete[] logCharsAllocated;
  }
};

//-----------------------------------------------------------------------------
// Log Configurator
//
// Centralized object that can configure and enumerate all the channels.

class ConfiguratorPlugin {
 public:
  ConfiguratorPlugin();
  virtual ~ConfiguratorPlugin();

  // Modify the channel level if it is set, otherwise leave it as-is.
  virtual void RestoreChannelLevel(const char* name, Level& level) = 0;

  // Sets the channel level
  virtual void SaveChannelLevel(const char* name, Level level) = 0;
};

class Configurator {
  friend class Channel;
  friend class OutputWorker;

  Configurator(); // Call GetInstance() to get the singleton instance.

 public:
  // Get singleton instance for logging configurator
  static Configurator* GetInstance();

  ~Configurator();

  void SetGlobalMinimumLogLevel(Level level);

  inline void SilenceLogging() {
    // Set the minimum logging level higher than any actual message.
    SetGlobalMinimumLogLevel(Level::Count);
  }

  // Sets the ConfiguratorPlugin for this Configurator to use.
  void SetPlugin(std::shared_ptr<ConfiguratorPlugin> plugin);

  // Get all channels - note channels do not necessarily have unique names
  void GetChannels(std::vector<std::pair<std::string, Level>>& channels);

  // Set all channels with channelName to level.
  // Cann disable a channel by using Level::Disabled.
  void SetChannelLevel(const std::string& channelName, Level level);

  // Internal: Invoked through callbacks
  void OnChannelLevelChange(const char* channelName, Log_Level_t level);

  // Internal: Load log level for a channel from disk, set all channels with this name to this level
  void RestoreChannelLogLevel(const char* channelName);

  // Internal: Load log level for a channel from disk, set this channel to this level
  void RestoreChannelLogLevel(ChannelNode* channelNode);

  // Internal: Iterate through all channels and store them
  void RestoreAllChannelLogLevels();

  // Maps to OutputWorker::SetOutputPluginChannels. See that for documentation.
  void SetOutputPluginChannels(
      const char* outputPluginName,
      const std::vector<std::string>& channelNames);

  // Maps to OutputWorker::SetChannelOutputPlugins. See that for documentation.
  void SetChannelOutputPlugins(
      const char* channelName,
      const std::vector<std::string>& outputPluginNames);

  // Maps to OutputWorker::SetChannelSingleOutput. See that for documentation.
  void SetChannelSingleOutput(const char* channelName, const char* outputPluginName);

 private:
  void RestoreAllChannelLogLevelsNoLock();

  uint32_t GlobalMinimumLogLevel;
  std::shared_ptr<ConfiguratorPlugin> Plugin;

  void SetChannelLevelNoLock(const std::string& channelName, Level level, bool overrideUser);
};

// Convenience function: ovrlog::Flush();
inline void Flush() {
  OutputWorker::GetInstance()->Flush();
}

} // namespace ovrlog
