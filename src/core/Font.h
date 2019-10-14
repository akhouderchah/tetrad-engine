#pragma once

#include <string>

#include "core/BaseTypes.h"
#include "core/GlTypes.h"

namespace tetrad
{

/**
 * @brief
 */
class Font
{
 public:
  Font();

  /**
   * @brief Load a font asset for use by TextComponents
   */
  bool Load(const std::string &fontPath, uint32_t pixelHeight = 42u);

  /**
   * @brief Unload font asset information
   */
  void Unload();

  struct CharInfo
  {
    glm::ivec2 Size;      // Size of glyph
    glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
    GLuint TextureID;     // ID handle of the glyph texture
    signed long Advance;  // Offset to advance to next glyph
  };

  /**
   * @brief Get the FontChar instance of a particular character
   *
   * @note Array bounds are only checked in debug mode
   */
  const CharInfo &GetChar(uint8_t c) const;

  inline const CharInfo &operator[](uint8_t c) const { return GetChar(c); }

  static Font &GetDefaultFont();

 private:
  static Font s_DefaultFont;

  bool m_IsLoaded;

  // TODO this is temporary, won't work with unicode!!!
  CharInfo m_CharInfo[128];
};

}  // namespace tetrad
