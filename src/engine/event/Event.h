#pragma once

#include "engine/event/Constants.h"

namespace tetrad
{

/**
 * @brief Structure containing information for a game event
 *
 * Contains information indicating what the new state for a
 * particular game event is. Used mostly by the EventSystem
 * and ObserverComponents.
 */
struct Event
{
  Event() : event(EGE_NONE), state(EGameState::DISABLED) {}
  EGameEvent event;
  EEventAction action;
  EGameState state;
};

}  // namespace tetrad
