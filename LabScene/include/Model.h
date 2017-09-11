#pragma once

#include <glad\glad.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <unordered_map>

#include <Mesh.h>
#include <Texture2D.h>

class Model
{
public:
	std::vector<Mesh> meshes;
	std::unordered_map<std::string, Texture2D> texturesLoaded;  //record which texture is already loaded
	std::string directory;    //this member is to store the model's root directory, because some image of texture's path is relative path

	Model(const std::string &path);
	void draw(Shader shader) const;

private:
	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
};

Model::Model(const std::string &path)
{
	loadModel(path);
}

void Model::draw(Shader shader) const
{
	//model is just a set of meshes
	for (const auto &m : meshes)
		m.draw(shader);
}

void Model::loadModel(const std::string &path)
{
	Assimp::Importer importer;
	//we postprocess all mesh in the original model to triangle and flip the images
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	//root directory of model
	directory = path.substr(0, path.rfind('/') + 1);
	//process the nodes recursively
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	//process mesh for every child
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		//the node->mMeshes stored the index of meshes in this node
		//the data is stored in scene->mMeshes
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (size_t i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture2D> textures;
	//process vertices
	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vert;
		//position
		glm::vec3 v3;
		v3.x = mesh->mVertices[i].x;
		v3.y = mesh->mVertices[i].y;
		v3.z = mesh->mVertices[i].z;
		vert.position = v3;
		//normal
		v3.x = mesh->mNormals[i].x;
		v3.y = mesh->mNormals[i].y;
		v3.z = mesh->mNormals[i].z;
		vert.normal = v3;
		//texCoord
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 v2;
			v2.x = mesh->mTextureCoords[0][i].x;
			v2.y = mesh->mTextureCoords[0][i].y;
			vert.texCoord = v2;
		}
		else
			vert.texCoord = glm::vec2(0.0f, 0.0f);
		//the processed data store in the vector
		vertices.push_back(vert);
	}
	//process indices
	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
	//process material
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture2D> diffMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
	std::vector<Texture2D> specMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specMaps.begin(), specMaps.end());
	
	return Mesh(vertices, indices, textures);
}

std::vector<Texture2D> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
{
	std::vector<Texture2D> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); ++i)
	{
		//the image's path
		aiString str;
		//get the relative path of this texture image
		mat->GetTexture(type, i, &str);
		std::string p(str.C_Str());
		if (texturesLoaded.find(p) != texturesLoaded.end())
		{
			textures.push_back(texturesLoaded[p]);
			std::cout << "Texture: " << p << " already loaded!" << std::endl;
			continue;
		}
		else
		{
			Texture2D tex;
			std::string imagePath = directory + p;
			//generate opengl texture
			tex.loadFromFile(imagePath);
			tex.type = typeName;
			tex.path = p;
			textures.push_back(tex);
			texturesLoaded.insert({ tex.path, tex });
			std::cout << "Texture: " << p << " load successfully!" << std::endl;
		}
	}
	return textures;
}