#pragma once

#include <random>

#include "core/BaseTypes.h"
#include "core/Platform.h"

namespace tetrad {

/** @brief Class to generate random numbers. */
class Random
{
 private:
  enum RandomWeight
  {
    ERW_VeryLikely,
    ERW_Likely,
    ERW_Average,
    ERW_Unlikely,
    ERW_Impossibru = 6
  };

 public:
  Random();

  /** @brief Provides a custom seed to the random engine.
   *
   * @note As the engine is seeded by random_device on construction,
   * this should mainly be used for cases where reproduciblity is needed.
   */
  void Reseed(unsigned long seed);

  /** @brief Get a random integer within a uniformly distributed inclusive range. */
  int GetRand(int min, int max);
  inline int GetRand(int max) { return GetRand(0, max); }

  /** @brief Get a random float within a uniformly distributed inclusive range. */
  float GetRand(float start, float end);
  inline float GetRand(float max) { return GetRand(0.f, max); }

  bool WillHappen(RandomWeight weight);  // Returns whether or not something will happen,
                                         // given a weight

  /** @brief Returns a static instance of Random.
   *
   * @note the std::random_engines are not thread-safe. This method
   *       should be modified for concurrent code to take a thread id,
   *       such that each thread uses its own instance.
   */
  static Random &GetGlobalInstance()
  {
    static Random rand;
    return rand;
  }

 private:
  std::mt19937 m_RandomEngine;
  static std::random_device s_rd;
};

}  // namespace tetrad
