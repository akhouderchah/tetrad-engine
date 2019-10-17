#include "core/Platform.h"
#include "editor/Editor.h"
#include "engine/ecs/EntityManager.h"

using namespace tetrad;

int main()
{
  if (!programInitialize())
  {
    LOG_ERROR("Failed to do platform-specific program initialization!");
    return -1;
  }
  LOG_DEBUG("Platform-specific editor initialization successful\n");

  Editor editor;
  GameAttributes attributes(ScreenAttributes(1280, 960, false, true, false, 4, 4, 4,
                                             "Tetrad Editor " + kVersionString),
                            MouseMode::NORMAL);
  if (!editor.Initialize(attributes))
  {
    return -1;
  }
  LOG_DEBUG("Editor initialization successful\n");

  editor.Run();
  editor.Shutdown();

  return 0;
}
