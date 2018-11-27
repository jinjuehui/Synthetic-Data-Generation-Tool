#pragma once
#include <glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Shader.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};


	struct Texture
	{
		unsigned int id;
		std::string type;
		std::string path;
	};

	class Mesh
	{
	public:
		std::vector<Vertex> Vertecies;
		std::vector<unsigned int> Indicies;
		std::vector<Texture> Textures;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, std::vector<Texture> Texture);
		void Draw(Shader shader);

	private:
		unsigned int VAO, VBO, EBO;
		void setupMesh();
	};

	class Model
	{
	public:
		Model(char *path);
		void Draw(Shader shader);
	private:
		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		std::string directory;
		void loadModel(std::string path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	};

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	unsigned int TextureFromFile(const char *path, const std::string &directory);