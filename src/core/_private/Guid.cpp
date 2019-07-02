#include "core/Guid.h"

/**
 * GUID class family static definitions
 */
template <class T, class IDType>
IDType GUID<T, IDType>::s_currentID = 1;

