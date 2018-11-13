#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	:m_count(count)
{
	GLCall(glGenBuffers(1, &Index_buffer_ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Index_buffer_ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*(sizeof(unsigned int)), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1,&Index_buffer_ID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Index_buffer_ID));
}

void IndexBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));
}

