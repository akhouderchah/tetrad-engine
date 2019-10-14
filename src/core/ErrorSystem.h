#pragma once

#include <string>

#include "core/LogSystem.h"
#include "core/Platform.h"

namespace tetrad
{

class Game;

/**
 * @brief Encapsulation of error functions
 */
class ErrorSystem
{
 public:
  static void SetCurrentGame(Game*);
  static void ForceExitGame();

 private:
  static Game* s_pGame;
};

/**
 * @brief Macro to handle errors (log them and then decide what to do)
 */
#define LOG_ERROR(stream) _LOG_ERROR(g_MainLog, Log::EIL_NORMAL, stream);

#define LOG_ERROR_EXIT(stream) \
  LOG_ERROR(stream);           \
  ErrorSystem::ForceExitGame();

void _assert_exit(std::string cond, const char* file, int line);

#define TRD_ASSERT(cond) (void)((cond) || (_assert_exit(#cond, __FILE__, __LINE__), 0))

#define RELEASE_ASSERT(cond) TRD_ASSERT(cond)

#ifdef _DEBUG
#define DEBUG_ASSERT(cond) TRD_ASSERT(cond)
#else
#define DEBUG_ASSERT(cond)
#endif

/**
 * @brief Assert in debug mode, check in release mode
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
