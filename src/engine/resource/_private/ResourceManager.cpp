#include "engine/resource/ResourceManager.h"

#include "core/Log.h"
#include "core/Package.h"
#include "core/Paths.h"
#include "engine/render/DrawComponent.h"
#include "engine/resource/Font.h"

DISABLE_WARNINGS()
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
ENABLE_WARNINGS()

using namespace glm;

namespace tetrad {

// Static member variable initialization
std::unordered_map<std::string, GLuint> ResourceManager::s_Textures;
std::unordered_map<std::string, ModelResource> ResourceManager::s_Models;
std::unordered_map<std::string, Font> ResourceManager::s_Fonts;

typedef PackageFormat::TextureHeader TextureHeader;

ResourceManager::ResourceManager() {}

ModelResource ResourceManager::LoadShape(ShapeType type)
{
  switch (type)
  {
    case ShapeType::PLANE:
      return LoadModel(MODEL_PATH + "plane.obj");

    case ShapeType::CUBE:
      return LoadModel(MODEL_PATH + "cube.obj");

    default:
      return ModelResource{0, 0, 0};
  }
}

ModelResource ResourceManager::LoadModel(std::string path)
{
  auto iter = s_Models.find(path);
  if (iter == s_Models.end())
  {
    // Load assimp scene from file
    Assimp::Importer importer;
    const aiScene *pScene = importer.ReadFile(path, 0);
    if (!pScene)
    {
      LOG_ERROR(importer.GetErrorString() << "\n");
      return ModelResource{0, 0, 0};
    }

    // Get the first (and usually the only) mesh in a scene
    // @TODO should we try to extract multiple meshes from a scene?
    const aiMesh *pMesh = pScene->mMeshes[0];

    // Setup vertices
    std::vector<DrawComponent::Vertex> vertices;
    vertices.reserve(pMesh->mNumVertices);
    for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
    {
      aiVector3D pos = pMesh->mVertices[i];
      aiVector3D normal = pMesh->mNormals[i];
      aiVector3D UV = pMesh->mTextureCoords[0][i];  // NOTE: only using 1st set of UVs

      vertices.push_back({vec3(pos.x, pos.y, pos.z), vec3(normal.x, normal.y, normal.z),
                          vec2(UV.x, UV.y)});
    }

    // Setup indices
    // NOTE: Assumes faces consist only of triangles (no quads),
    // which is a very fair assumption for games
    std::vector<unsigned int> indices;
    indices.reserve(3 * pMesh->mNumFaces);
    for (unsigned int i = 0; i < pMesh->mNumFaces; ++i)
    {
      indices.push_back(pMesh->mFaces[i].mIndices[0]);
      indices.push_back(pMesh->mFaces[i].mIndices[1]);
      indices.push_back(pMesh->mFaces[i].mIndices[2]);
    }

    GLuint VBO, IBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(DrawComponent::Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    s_Models[path] = ModelResource{VBO, IBO, (GLsizei)indices.size()};
    return s_Models[path];
  }
  return iter->second;
}

Font &ResourceManager::LoadFont(std::string fontPath)
{
  auto iter = s_Fonts.find(fontPath);
  if (iter == s_Fonts.end())
  {
    Font font;
    if (!font.Load(fontPath))
    {
      return Font::GetDefaultFont();
    }

    s_Fonts[fontPath] = font;
  }
  return s_Fonts[fontPath];
}

void ResourceManager::UnloadFont(std::string fontPath)
{
  auto iter = s_Fonts.find(fontPath);
  if (iter != s_Fonts.end())
  {
    iter->second.Unload();
    s_Fonts.erase(iter);
  }
}

}  // namespace tetrad
