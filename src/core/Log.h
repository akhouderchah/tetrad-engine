// TODO - Controlled creation of log files (as opposed to regular static init)

#pragma once

#include "core/LogInternals.h"
#include "core/Platform.h"

namespace tetrad {

extern Log g_MainLog;

/** @brief Log a stream with a custom title. Otherwise behaves as a normal info log. */
#define LOG_SPECIAL(title, stream) _LOG(g_MainLog, Log::EIL_NORMAL, title, stream)

#define LOG_DEBUG(stream)   _LOG_DEBUG(g_MainLog, Log::EIL_NORMAL, stream)
#define LOG(stream)         LOG_SPECIAL("Info ", stream)
#define LOG_WARNING(stream) LOG_SPECIAL("Warn ", stream)
#define LOG_ERROR(stream)   _LOG_ERROR(g_MainLog, Log::EIL_NORMAL, "Error", stream)
#define LOG_FATAL(stream)                                  \
  _LOG_ERROR(g_MainLog, Log::EIL_NORMAL, "Fatal", stream), \
      ExitHook::Instance()->TriggerTermination(ExitReason::LOG_FATAL)

/** @brief Assert a condition in all builds. */
#define RELEASE_ASSERT(cond) TRD_ASSERT(cond)
/** @brief Assert a condition only in debug builds. Else noop. */
#ifdef _DEBUG
#define DEBUG_ASSERT(cond) TRD_ASSERT(cond)
#else
#define DEBUG_ASSERT(cond)
#endif

/** @brief Assert in debug mode, check in release mode.
 *
 * If cond is untrue, a debug program will crash with an assertion failed
 * mesage, while a release program will return from the function with the
 * value ret.
 */
#ifndef _DEBUG
#define ASSERT_CHECK(cond, ret) \
  if (!(cond)) return ret
#else
#define ASSERT_CHECK(cond, ret) DEBUG_ASSERT(cond)
#endif

}  // namespace tetrad
