#include "core/Platform.h"
#include "engine/resource/ResourceManager.h"

DISABLE_WARNINGS()
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_TGA
#include "stb_image.h"
ENABLE_WARNINGS()

#include "core/Log.h"

namespace tetrad {

GLuint ResourceManager::LoadTexture(const std::string &str, TextureType type)
{
  auto iter = s_Textures.find(str);

  // Shortcut if we already loaded the texture
  if (iter != s_Textures.end())
  {
    return iter->second;
  }

  // Actually load the texture
  int comp, h, w;
  unsigned char *pImage;
  GLuint tex = 0;

  static bool firstRun = true;
  if (firstRun)
  {
    // TODO - do this is some init function!
    stbi_set_flip_vertically_on_load(true);
    firstRun = false;
  }

  if (type == TextureType::RGBA)
  {
    pImage = stbi_load(str.c_str(), &w, &h, &comp, STBI_rgb_alpha);
  }
  else
  {
    pImage = stbi_load(str.c_str(), &w, &h, &comp, STBI_rgb);
  }

  if (pImage == nullptr)
  {
    LOG_DEBUG("Failed to load texture file: " << str << "\n");
    goto error;
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  switch (comp)
  {
    case 1:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pImage);
      break;
    case 3:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   pImage);
      break;
    default:
      LOG_DEBUG("Invalid texture format in file: " << comp << " different components in "
                                                   << str << "\n");
      goto error;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(pImage);
  s_Textures[str] = tex;
  return tex;

error:
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(pImage);
  if (tex)
  {
    glDeleteTextures(1, &tex);
  }
  return 0;
}

/*
GLuint ResourceManager::LoadTexture(const std::string &name, TextureHeader *pHdr)
{
        auto iter = s_Textures.find(str);

        // Shortcut if we already loaded the texture
        if(iter != s_Textures.end())
        {
                return iter->second;
        }

        // Actually load the texture
        int comp, h, w;
        unsigned char *pImage;
        GLuint tex = 0;

        if(pHdr->Flags & TextureHeader::HAS_ALPHA)
        {
                pImage = stbi_load(str.c_str(), &w, &h, &comp, STBI_rgb_alpha);
        }
        else
        {
                pImage = stbi_load(str.c_str(), &w, &h, &comp, STBI_rgb);
        }

        if(pImage == nullptr)
        {
                // TODO log error
                goto error;
        }

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if(comp == 3)
        {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
pImage);
        }
        else if(comp == 4)
        {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
GL_UNSIGNED_BYTE, pImage);
        }
        else
        {
                // TODO log error
                goto error;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(pImage);
        s_Textures[str] = tex;
        return tex;

error:
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(pImage);
        if(tex)
        {
                glDeleteTextures(1, &tex);
        }
        return 0;
}
*/

bool ResourceManager::UnloadTexture(const std::string &str)
{
  auto iter = s_Textures.find(str);
  if (iter != s_Textures.end())
  {
    glDeleteTextures(1, &(iter->second));
    s_Textures.erase(iter);
    return true;
  }

  return false;
}

void ResourceManager::UnloadAllTextures()
{
  for (auto tex : s_Textures)
  {
    glDeleteTextures(1, &(tex.second));
  }

  s_Textures.clear();
}

}  // namespace tetrad
