#include "engine/resource/Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "core/Log.h"
#include "core/Paths.h"
#include "engine/resource/ResourceManager.h"

namespace tetrad {
Font Font::s_DefaultFont;

Font::Font() : m_IsLoaded(false) {}

bool Font::Load(const std::string &fontPath, uint32_t pixelHeight)
{
  if (m_IsLoaded)
  {
    LOG_DEBUG("Font cannot be loaded twice. Failed to load font " << fontPath << "\n");
    return false;
  }

  FT_Library ft;
  if (FT_Init_FreeType(&ft))
  {
    LOG_ERROR("Error initializing FreeType!\n");
    return false;
  }

  FT_Face face;
  if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
  {
    LOG_ERROR("Failed to load font: " << fontPath << "\n");
    return false;
  }

  // Set font size
  FT_Set_Pixel_Sizes(face, 0, pixelHeight);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Generate textures for relevant characters
  // TODO come up with system for unicode strings!!!
  for (GLubyte c = 0; c < 128; c++)
  {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
      LOG_ERROR("Failed load glyph '" << (char)c << "' in font: " << fontPath << "\n");
      continue;
    }

    // TODO place all textures into single larger texture!
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Store character for later use
    CharInfo character = {glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                          texture, face->glyph->advance.x};
    m_CharInfo[c] = character;
  }

  if (FT_Done_Face(face))
  {
    LOG_ERROR("Failed to unload font\n");
    return false;
  }
  if (FT_Done_FreeType(ft))
  {
    LOG_ERROR("Error shutting down FreeType library object!\n");
    return false;
  }

  m_IsLoaded = true;
  return true;
}

void Font::Unload()
{
  if (m_IsLoaded)
  {
    for (GLubyte c = 0; c < 128; c++)
    {
      glDeleteTextures(1, &m_CharInfo[c].TextureID);
    }

    m_IsLoaded = false;
  }
}

const Font::CharInfo &Font::GetChar(uint8_t c) const
{
#ifdef _DEBUG
  if (c >= 128u)
  {
    LOG_ERROR("Invalid index: '" << (char)c << "' into font!\n");
    return m_CharInfo[(uint8_t)'x'];
  }
  else
#endif
  {
    return m_CharInfo[c];
  }
}

Font &Font::GetDefaultFont()
{
  if (!s_DefaultFont.m_IsLoaded)
  {
    CharInfo character = {
        glm::ivec2(1, 1), glm::ivec2(1, 1),
        ResourceManager::LoadTexture(TEXTURE_PATH + "Black.tga", TextureType::RGB), 1};
    for (GLubyte c = 0; c < 128; c++)
    {
      // Store character for later use
      s_DefaultFont.m_CharInfo[c] = character;
    }
  }

  return s_DefaultFont;
}

}  // namespace tetrad
