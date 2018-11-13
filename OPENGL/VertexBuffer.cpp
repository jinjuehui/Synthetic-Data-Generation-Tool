#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &Vertex_buffer_ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, Vertex_buffer_ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1,&Vertex_buffer_ID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, Vertex_buffer_ID));
}

void VertexBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
}
