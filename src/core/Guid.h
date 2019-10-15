#pragma once

namespace tetrad {

/**
 * @brief Set of classes to generate GUIDs for different purposes.
 *
 * @in - T - The GUID "purpose" (the set of classes (base class perhaps) that will use the
 *GUID)
 * @in - IDType - The data type of the ID (most likely an integer type, although it could
 *be specialized different types)
 *
 *	@note The base implementation only generates IDs, it does not reclaim them, even
 *if an ID is no longer used.
 */
template <class T, class IDType>
class GUID
{
 public:
  GUID() = delete;
  ~GUID() = delete;

  static IDType GenerateID()
  {
    assert(s_currentID + 1 > s_currentID);  // Check for overflow
    return s_currentID++;
  }

 private:
  static IDType s_currentID;
};

}  // namespace tetrad
