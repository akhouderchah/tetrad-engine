#pragma once

#include <unordered_map>

#include "core/BaseTypes.h"
#include "core/GlTypes.h"

namespace tetrad
{

class Font;

/**
 */
struct ModelResource
{
  GLuint m_VBO;
  GLuint m_IBO;
  GLsizei m_IndexCount;
};

/**
 * @brief Class to make sure resources are only loaded as needed
 */
class ResourceManager
{
 public:
  ResourceManager();

  //
  // Texture functions
  // @TODO remove non-package version
  static GLuint LoadTexture(const std::string &str, TextureType type);
  // static GLuint LoadTexture(const std::string &name, PackageFormat::TextureHeader
  // *pHdr);
  static bool UnloadTexture(const std::string &str);
  static void UnloadAllTextures();

  //
  // Model functions
  //
  static ModelResource LoadShape(ShapeType type);
  static ModelResource LoadModel(std::string path);

  //
  // Font functions
  //

  static Font &LoadFont(std::string fontPath);
  static void UnloadFont(std::string fontPath);

 private:
  static std::unordered_map<std::string, GLuint> s_Textures;
  static std::unordered_map<std::string, ModelResource> s_Models;
  static std::unordered_map<std::string, Font> s_Fonts;
};

}  // namespace tetrad
