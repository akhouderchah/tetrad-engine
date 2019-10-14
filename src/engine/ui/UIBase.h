#pragma once

#include "core/BaseTypes.h"

namespace tetrad
{

typedef RectangularBound<uint8_t, uint32_t> UIRectangleBound_t;

/*
   +----------+-------------------------------------------------------
   | Priority | General use-case
   +----------+-------------------------------------------------------
   |    0     | UIViewports and elements pinned to back
   |    1     | Default priority (most elements have this priority)
   |    2     | Elements pinned to the top (e.g. a toolbar)
   +----------+-------------------------------------------------------
*/
#define UI_PRIORITY_COUNT 3  // TODO - is this the right amount?
#define UI_PRIORITY_DEFAULT 1
#define UI_PRIORITY_TOP (UI_PRIORITY_COUNT - 1)

static_assert(UI_PRIORITY_DEFAULT < UI_PRIORITY_COUNT,
              "UI_DEFAULT_PRIORITY must be less than"
              " the number of priority levels!");

}  // namespace tetrad
