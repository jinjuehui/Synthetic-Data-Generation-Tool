#include "VertexBuffer.h"
#include "Renderer.h"


VertexBuffer::VertexBuffer(unsigned int Vertex_buffer_ID,const void* data, unsigned int size)
{
	VBO = Vertex_buffer_ID;
	GLCall(glGenBuffers(1, &Vertex_buffer_ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, Vertex_buffer_ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(const void* data,int number_of_elements,size_t element_size, int layout,int size_of_vertex, int stride, int offset)
{
	NOE = number_of_elements;
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, number_of_elements*element_size, data, GL_STATIC_DRAW));
	GLCall(glVertexAttribPointer(layout, size_of_vertex, GL_FLOAT, GL_FALSE, stride, (void*)offset));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glBindVertexArray(0));
}

VertexBuffer::VertexBuffer(const void* data, 
						   const void* EBO_data,
						   int number_of_elements,
						   size_t element_size,
						   int number_of_index,
						   size_t index_size,
						   std::map<std::string,int> AttribPointer)
{
	NOE = number_of_index;
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glGenBuffers(1, &EBO));

	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, number_of_elements*element_size, data, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_of_index*index_size,EBO_data,GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(AttribPointer["layout_0"], AttribPointer["size_of_vertex_0"], GL_FLOAT, GL_FALSE, AttribPointer["stride_0"], (void*)AttribPointer["offset_0"]));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(AttribPointer["layout_1"], AttribPointer["size_of_vertex_1"], GL_FLOAT, GL_FALSE, AttribPointer["stride_1"], (void*)AttribPointer["offset_1"]));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));

}

VertexBuffer::VertexBuffer(const void* data,
						   const void* EBO_data,
						   int number_of_elements,
						   size_t element_size,
						   int number_of_index,
						   size_t index_size,
						   std::map<std::string, int> AttribPointer,
						   std::string key)//<--any value to overload the function
{
	NOE = number_of_index;
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glGenBuffers(1, &EBO));

	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, number_of_elements*element_size, data, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_of_index*index_size, EBO_data, GL_STATIC_DRAW));
	//TODO auto parsing the Attribpointer map
	GLCall(glVertexAttribPointer(AttribPointer["layout_0"], AttribPointer["size_of_vertex_0"], GL_FLOAT, GL_FALSE, AttribPointer["stride_0"], (void*)AttribPointer["offset_0"]));
	GLCall(glEnableVertexAttribArray(0));
	if (AttribPointer.count("layout_1"))
	{
		std::cout << "layout 1 found!" << std::endl;
		GLCall(glVertexAttribPointer(AttribPointer["layout_1"], AttribPointer["size_of_vertex_1"], GL_FLOAT, GL_FALSE, AttribPointer["stride_1"], (void*)AttribPointer["offset_1"]));
		GLCall(glEnableVertexAttribArray(1));
	}

	if (key == "generate texture")
	{
		GLCall(glGenTextures(1, &TEXTURE));
		GLCall(glBindTexture(GL_TEXTURE_2D, TEXTURE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));
	}
}

VertexBuffer::~VertexBuffer()
{
	std::cout << "destructor called!" << std::endl;
	if(&VAO)
	GLCall(glDeleteBuffers(1,&VAO));
	if(&VBO)
	GLCall(glDeleteBuffers(1, &VBO));
	if(&EBO)
	GLCall(glDeleteBuffers(1, &EBO));
}

void VertexBuffer::Bind(std::string key) const
{
	if (key=="bind_vertex_buffer")
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLCall(glBindVertexArray(VAO));
	}
	else if(key=="bind_texture")
	{
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, TEXTURE));
		GLCall(glBindVertexArray(VAO));
	}
}

void VertexBuffer::Draw(std::string key) const
{
	if (key=="draw_elements")
	{
		//GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLCall(glBindVertexArray(VAO));
		GLCall(glDrawElements(GL_TRIANGLES, NOE, GL_UNSIGNED_INT, 0));
	}
	else if(key=="draw_arrays")
	{
		GLCall(glBindVertexArray(VAO));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, NOE));
	}
}

void VertexBuffer::load_texture_image(GLenum target,
									  int internalformat,
									  int width,
									  int height,
									  int border,
									  GLenum format,
									  const GLvoid* data) const
{
	if (data) {
		try
		{

			GLCall(glTexImage2D(target,	0,internalformat,width,height,border,format,GL_UNSIGNED_BYTE,data));
			//.................texture target,mipmap level,legacy,format,and datatype,data
			//GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	
		}
		catch (const std::exception&)
		{
			
		}
	}
	else
		std::cout << "failed to load image" << std::endl;
}

void VertexBuffer::UnBind() const
{
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
}
