#pragma once
#include "Shader.h"
#include <map>
class VertexBuffer
{
	unsigned int VAO, VBO, EBO, NOE, TEXTURE;
public:
	VertexBuffer(unsigned int Vertex_buffer_ID,
				 const void* data,
				 unsigned int size);
	VertexBuffer(const void* data,
		         int number_of_elements,
				 size_t element_size,
				 int layout,
		         int size_of_vertex,
		         int stride,
		         int offset);
	VertexBuffer(const void* data,
			     const void* EBO_data,
				 int number_of_elements,
				 size_t element_size,
				 int number_of_index,
				 size_t index_size,
				 std::map<std::string, int> AttribPointer);
	VertexBuffer(const void* data,
				 const void* EBO_data,
				 int number_of_elements,
				 size_t element_size,
				 int number_of_index,
				 size_t index_size,
				 std::map<std::string, int> AttribPointer,
				 std::string key);

	~VertexBuffer();
	void Bind(std::string key) const;
	void Draw(std::string key) const;
	void load_texture_image(GLenum target,int internalformat ,int width, int height, int border,GLenum format,const GLvoid* data) const;
	void UnBind() const;

};