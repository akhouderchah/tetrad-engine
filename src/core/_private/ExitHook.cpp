#include "core/ExitHook.h"

#include <cstdlib>

namespace tetrad {

ExitHook *ExitHook::Instance()
{
  static ExitHook instance;
  return &instance;
}

void ExitHook::TriggerTermination(ExitReason reason)
{
  for (auto iter = m_HookStack.rbegin(); iter != m_HookStack.rend(); ++iter)
  {
    (*iter)(reason);
  }
  exit(static_cast<int>(reason));
}

}  // namespace tetrad
