#include "core/Rand.h"

#include "core/Log.h"

namespace tetrad {
std::random_device Random::s_rd;

Random::Random() : m_RandomEngine(s_rd()) {}

void Random::Reseed(unsigned long seed) { m_RandomEngine.seed(seed); }

int Random::GetRand(int min, int max)
{
  std::uniform_int_distribution<int> dist(min, max);
  return dist(m_RandomEngine);
}

float Random::GetRand(float min, float max)
{
  std::uniform_real_distribution<float> dist(min, max);
  return dist(m_RandomEngine);
}

bool Random::WillHappen(RandomWeight weight)
{
  // Is W^2+1 < .95?
  // @TODO - Use real probability distribution!
  return (GetRand(0, (weight * weight) + 1) < 0.95f);
}

}  // namespace tetrad
