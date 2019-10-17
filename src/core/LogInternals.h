#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "core/Platform.h"

namespace tetrad {

/**
 * Set of macro functions to abstract away the Log class details.
 *
 * Logs are written to a file, and optionally written to console as well.
 *
 * The log macro naming convention is that the "general" macros are prefixed
 * with an underscore (general meaning made for use in other macros, not
 * directly by the user), while macros for the user are not prefixed by an
 * underscore.
 *
 * Note: Unix-based systems could do input redirection in place of this, but
 * this allows for finer control over where things are logged.
 *
 * Note: "Flexible" logging means the decision of what and when to write (debug
 * and info levels) happens at runtime. If LOG_FLEXIBLE is not #defined, that
 * decision will be made at compile-time instead, losing flexibility but
 * removing extra branches from the codebase.
 */
//#define LOG_FLEXIBLE
#ifdef _DEBUG
#define LOG_ALLOW_DEBUG
#endif
//#define LOG_CONSOLE
#define LOG_DEBUG_CONSOLE

extern std::ostream* g_pConsoleStream;
extern std::ostream* g_pDebugConsoleStream;

const std::string GetTimeStr();

#define LOG_HEADER(title) "[" << title << "] " << GetTimeStr() << " " \
  << __FILE_RELATIVE__ << ":" << __LINE__ << " - "

#define LOG_DEBUG_HEADER LOG_HEADER("Debug")

#ifdef LOG_CONSOLE
#define _CONSOLE_PRINT(stream) , (*::tetrad::g_pConsoleStream) << stream
#else
#define _CONSOLE_PRINT(stream)
#endif

#ifdef LOG_DEBUG_CONSOLE
#define _CONSOLE_DEBUG_PRINT(stream) , (*::tetrad::g_pDebugConsoleStream) << stream
#else
#define _CONSOLE_DEBUG_PRINT(stream)
#endif

#ifdef LOG_FLEXIBLE
#define _LOG(logger, infoLevel, title, stream) \
  if (logger.GetMinLevel() > infoLevel)        \
  {                                            \
  }                                            \
  else                                         \
    logger.GetStream(infoLevel) << LOG_HEADER(title) << stream _CONSOLE_PRINT(stream)
#define _LOG_DEBUG(logger, infoLevel, stream)                     \
  if (!logger.DebugEnabled() || logger.GetMinLevel() > infoLevel) \
  {                                                               \
  }                                                               \
  else                                                            \
    logger.GetStream(infoLevel) << LOG_DEBUG_HEADER << stream _CONSOLE_DEBUG_PRINT(stream)

#else  // LOG_FLEXIBLE
#define _LOG(logger, infoLevel, title, stream) \
  logger.GetImmediate() << LOG_HEADER(title) << stream _CONSOLE_PRINT(stream)
#ifdef LOG_ALLOW_DEBUG
#define _LOG_DEBUG(logger, infoLevel, stream) \
  logger.GetImmediate() << LOG_DEBUG_HEADER << stream _CONSOLE_DEBUG_PRINT(stream)
#else  // LOG_ALLOW_DEBUG
#define _LOG_DEBUG(logger, infoLevel, stream)
#endif  // LOG_ALLOW_DEBUG
#endif  // LOG_FLEXIBLE

// We want all errors to be logged, so error writes are unconditional.
#define _LOG_ERROR(logger, infoLevel, title, stream)          \
  logger.GetStream(infoLevel) << LOG_HEADER(title) << stream, \
      (*::tetrad::g_pConsoleStream) << LOG_HEADER(title) << stream

void _assert_exit(std::string cond, const char* file, int line);
#define TRD_ASSERT(cond) (void)((cond) || (_assert_exit(#cond, __FILE__, __LINE__), 0))

/** @brief Class that handles the heavy-lifting of logging.
 *
 * Has two write streams: Delayed and Immediate. Immediate streams write
 * directly to files, while delayed streams are string streams that will
 * periodically be written to files.
 *
 * @note Users should be using the #defines above instead of directly working
 * with this class.
 */
class Log
{
 public:
  enum EInfoLevel
  {
    EIL_NONE = -1,
    EIL_VERBOSE,
    EIL_NORMAL,
    EIL_IMPORTANT
  };

  Log(const std::string& logPath, EInfoLevel minLog = EIL_NORMAL,
      EInfoLevel maxDelay = EIL_NORMAL);
  ~Log();

  Log(const Log&) = delete;
  Log(Log&&) = delete;
  Log& operator=(const Log&) = delete;
  Log& operator=(Log&&) = delete;

  EInfoLevel GetMinLevel() const { return m_MinLogLevel; }
  bool DebugEnabled() const { return m_WriteDebug; }

  std::ostream& GetStream(EInfoLevel infoLevel);
  std::ostream& GetImmediate() { return *m_pImmediateStream; }

 private:
  EInfoLevel m_MinLogLevel;    // We won't log any messages below this level
  EInfoLevel m_MaxDelayLevel;  // We won't do delayed writes on anything above this level
  static const std::streampos s_MAX_DELAY_SIZE;
  bool m_WriteDebug;
  bool m_HasOpenFile;
  std::ostream* m_pImmediateStream;
  std::ostringstream* m_pDelayedStream;
};

}  // namespace tetrad
