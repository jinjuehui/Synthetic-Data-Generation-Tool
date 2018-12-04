#include "Mesh.h"
#include "Renderer.h"
#include "stb_image.h"



Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, std::vector<Texture> textures)
{
	this->Vertecies = vertices;//it looks like there s no problem without "this" pointer
	this->Indicies = indicies;
	this->Textures = textures;
	setupMesh();
}

void Mesh::setupMesh()
{
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glGenBuffers(1, &EBO));

	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, Vertecies.size()*sizeof(Vertex), &Vertecies[0],GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,Indicies.size()*sizeof(unsigned int),&Indicies[0],GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(4));
	GLCall(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float))));


	GLCall(glBindVertexArray(0));
}

void Mesh::Draw(Shader shader)
{
	unsigned int diffuseNr(1);
	unsigned int specularNr(1);
	unsigned int normalNr(1);
	unsigned int heightNr(1);

	for (unsigned int i = 0; i < Textures.size(); i++ )
	{
		GLCall(glActiveTexture(GL_TEXTURE0+i));
		std::string number;
		std::string name = Textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);
		GLCall(shader.use());
		std::string name_number = name + number;
		GLCall(shader.setInt(name_number, i));
		GLCall(glBindTexture(GL_TEXTURE_2D, Textures[i].id));

	}


	GLCall(glBindVertexArray(VAO));//VAO specifies which buffer to draw
	GLCall(glDrawElements(GL_TRIANGLES,Indicies.size(),GL_UNSIGNED_INT,0));
	GLCall(glBindVertexArray(0));
	GLCall(glActiveTexture(GL_TEXTURE0););


}

Model::Model(char *path, bool gamma) : gammaCorrection(gamma)
{
	loadModel(path);
}

void Model::Draw(Shader shader)
{
	for (int i=0; i<meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(std::string const &path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene||scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));  //directory = mesh/nanosuit/  as the name only path
	processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	for (unsigned int i=0; i<node->mNumMeshes;i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene)); //transfer aiMesh* type to the Mesh type we defined ourselfs
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;
	std::vector<Texture> textures;

	//process in vertex
	for (unsigned int i = 0; i<mesh->mNumVertices; i++)
	{
		//process vertex
		Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		
		//process normals
		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		//process texture coordinates
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords.x= mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		//tangent
		vertex.Tangent.x = mesh->mTangents[i].x;
		vertex.Tangent.y = mesh->mTangents[i].y;
		vertex.Tangent.z = mesh->mTangents[i].z;

		//bitangent
		vertex.Bitangent.x = mesh->mBitangents[i].x;
		vertex.Bitangent.y = mesh->mBitangents[i].y;
		vertex.Bitangent.z = mesh->mBitangents[i].z;
		verticies.push_back(vertex);

	}

	//process indicies
	for (unsigned int i = 0; i<mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indicies.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());//insert a vector into another vector, don't use push_back but insert. 
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	
	return Mesh(verticies, indicies, textures);//convert all the members from ASSIMP to Mesh class


	//process materials
	//if (mesh->mMaterialIndex > 0)
	//{
	//	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	//	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//	textures.insert(textures.end(), diffuseMaps.begin(),diffuseMaps.end());

	//	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//}

	

}


unsigned int TextureFromFile(const char *path, const std::string &directory,bool gamma)
{
	std::string filename = std::string(path);//filename here is the .jpg .png... picture data name
	filename = directory + '/' + filename;//add the directory information for now its mesh/nanosuit/

	//openGL setting
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}



std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i=0; i<mat->GetTextureCount(type);i++)//for each texture type
	{
		aiString str;//inheret from the std::string class
		mat->GetTexture(type, i, &str);//assimp function, fill in the str to get the picture(texture .jpg,.png...) name
		bool skip = false;
		for (unsigned int j = 0; j<textures_loaded.size();j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)//if name is the same, that means the texture was loaded before
			{
				textures.push_back(textures_loaded[j]);//fill in the loaded texture
				skip = true;//do not need to generate another texture object to extract the information again
				break;//exit for loop
			}
		}

		if (!skip)
		{
			Texture texture;//a texture container
			texture.id = TextureFromFile(str.C_Str(),directory); //load texture pictures and initialize them in OpenGL, return the id 
			texture.type = typeName;//fill in the type name
			texture.path = str.C_Str();//fill in the path
			textures.push_back(texture);//fill in the textures container
			textures_loaded.push_back(texture);//fill in the loaded container to check whether the same texture was loaded before
		}
	}
	return textures;


}
