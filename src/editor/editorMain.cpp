#include "Platform.h"
#include "EntityManager.h"
#include "editor/Editor.h"

int main()
{
	if(!programInitialize())
	{
		LOG_ERROR("Failed to do platform-specific program initialization!");
		return -1;
	}
	DEBUG_LOG("Platform-specific editor initialization successful\n");

	Editor editor;
	GameAttributes attributes(1280, 960, "Tetrad Editor " + VERSION_STRING,
							  false, true, MouseMode::NORMAL, 4);
	if(!editor.Initialize(attributes))
	{
		return -1;
	}
	DEBUG_LOG("Editor initialization successful\n");

	editor.Run();
	editor.Shutdown();

	return 0;
}
