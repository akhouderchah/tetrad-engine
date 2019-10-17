#pragma once

#include <functional>
#include <vector>

namespace tetrad {

enum class ExitReason : int
{
  SUCCESS = 0,
  USER_TRIGGERED,
  ASSERT,
  LOG_FATAL
};

/** @brief Singleton to insert exit hooks prior to (most) program terminations.
 *
 * While atexit exists, this provides more control over non-normal program
 * terminations (e.g., handling signals in Linux that aren't SIGKILL or SIGSTOP)
 * and also allows such things as capturing lambdas to be used as hooks.
 */
class ExitHook
{
 public:
  /** @brief Thread-safe singleton accessor method. */
  static ExitHook *Instance();

  /** @brief Add function to be called during program termination. */
  void AddHook(std::function<void(ExitReason)> f) { m_HookStack.push_back(std::move(f)); }
  /** @brief Call exit hooks in a LIFO order and then terminate the program. */
  void TriggerTermination(ExitReason reason = ExitReason::USER_TRIGGERED);

 private:
  ExitHook() = default;
  ExitHook(const ExitHook &other) = delete;
  ExitHook(ExitHook &&other) = delete;
  ExitHook &operator=(const ExitHook &other) = delete;
  ExitHook &operator=(ExitHook &&other) = delete;

 private:
  std::vector<std::function<void(ExitReason)>> m_HookStack;
};

}  // namespace tetrad
