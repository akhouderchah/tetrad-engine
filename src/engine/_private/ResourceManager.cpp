#include "ResourceManager.h"
#include "DrawComponent.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Static member variable initialization
std::unordered_map<std::string, GLuint> ResourceManager::s_Textures;
std::unordered_map<std::string, std::tuple<GLuint, GLuint, GLsizei>> ResourceManager::s_Models;

using namespace glm;
typedef PackageFormat::TextureHeader TextureHeader;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

std::tuple<GLuint, GLuint, GLsizei> ResourceManager::LoadShape(ShapeType type)
{
	static bool isPlaneLoaded = false;
	static bool isCubeLoaded = false;

	switch(type)
	{
	case ShapeType::PLANE:
		return LoadModel(MODEL_PATH + "plane.obj");
		if(!isPlaneLoaded)
		{
			DrawComponent::Vertex vertices[] = {
				{ vec3(-1.f, -1.f, 0.f), vec3(0, 0, 1), vec2(1.f, 1.f) },
				{ vec3(1.f, -1.f, 0.f), vec3(0, 0, 1), vec2(0.f, 1.f) },
				{ vec3(1.f, 1.f, 0.f), vec3(0, 0, 1), vec2(0.f, 0.f) },
				{ vec3(-1.f, 1.f, 0.f), vec3(0, 0, 1), vec2(1.f, 0.f) }
			};

			unsigned int indices [] = { 2, 1, 0,
										0, 3, 2 };

			// TODO Error handling; also move most code out of switch case!
			GLuint VBO, IBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glGenBuffers(1, &IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			isPlaneLoaded = true;
			s_Models["PLANE"] = std::make_tuple(VBO, IBO, 6);
		}
		return s_Models["PLANE"];

	case ShapeType::CUBE:
		if(!isCubeLoaded)
		{
			return std::make_tuple(0, 0, 0);

		}
		return s_Models["CUBE"];

	default:
		return std::make_tuple(0, 0, 0);
	}
}

std::tuple<GLuint, GLuint, GLsizei> ResourceManager::LoadModel(std::string path)
{
	auto iter = s_Models.find(path);
	if(iter == s_Models.end())
	{
		// Load assimp scene from file
		Assimp::Importer importer;
		const aiScene *pScene = importer.ReadFile(path, 0);
		if(!pScene)
		{
			ERROR(importer.GetErrorString() << "\n", EEB_CONTINUE);
			return std::make_tuple(0, 0, 0);
		}

		// Get the first (and usually the only) mesh in a scene
		// @TODO should we try to extract multiple meshes from a scene?
		const aiMesh *pMesh = pScene->mMeshes[0];

		// Setup vertices
		std::vector<DrawComponent::Vertex> vertices;
		vertices.reserve(pMesh->mNumVertices);
		for(unsigned int i = 0; i < pMesh->mNumVertices; ++i)
		{
			aiVector3D pos = pMesh->mVertices[i];
			aiVector3D normal = pMesh->mNormals[i];
			aiVector3D UV = pMesh->mTextureCoords[0][i]; // NOTE: only using 1st set of UVs

			vertices.push_back({ vec3(pos.x, pos.y, pos.z),
						vec3(normal.x, normal.y, normal.z),
						vec2(UV.x, UV.y) });
		}

		// Setup indices
		// NOTE: Assumes faces consist only of triangles (no quads),
		// which is a very fair assumption for games
		std::vector<unsigned int> indices;
		indices.reserve(3 * pMesh->mNumFaces);
		for(unsigned int i = 0; i < pMesh->mNumFaces; ++i)
		{
			indices.push_back(pMesh->mFaces[i].mIndices[0]);
			indices.push_back(pMesh->mFaces[i].mIndices[1]);
			indices.push_back(pMesh->mFaces[i].mIndices[2]);
		}

		GLuint VBO, IBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER,
					 vertices.size() * sizeof(DrawComponent::Vertex),
					 &vertices[0],
					 GL_STATIC_DRAW);

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 indices.size() * sizeof(unsigned int),
					 &indices[0],
					 GL_STATIC_DRAW);

		s_Models[path] = std::make_tuple(VBO, IBO, indices.size());
		return s_Models[path];
	}
	return iter->second;
}

