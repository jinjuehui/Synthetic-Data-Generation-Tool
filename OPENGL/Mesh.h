#pragma once
#include <glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"


	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		//glm::vec2 TexCoords;
		//glm::vec3 Tangent;
		//glm::vec3 Bitangent;
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

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, std::vector<Texture> textures);
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies);

		void Draw(Shader shader,bool norm_key, bool texture_key, bool tangent_key);
		
	private:
		unsigned int VAO,VBO,EBO;
		void setupMesh();
	};

	class Model //this class works as a container to load meshes through ASSIMP and convert to the meshes we defined ourselves
	{
	public:
		Model(char *path,bool gamma = false);
		void Draw(Shader shader);
	private:
		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		std::string directory;
		bool gammaCorrection;
		bool norm_key = false;
		bool tangent_key = false;
		bool texture_key = false;

		void loadModel(std::string const &path);//load file use ASSIMP, {proocessNode{processMesh{fill in: 1.position, 2.normal, 3.tangent, 4.bitangent, 5.texture coordinate, 6.indices, 7.four types of textures{loadMaterialTextures}}}},
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	};

	unsigned int TextureFromFile(const char *path, const std::string &directory,bool gamma = false);