#pragma once
class VertexBuffer
{
	unsigned int Vertex_buffer_ID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();
	void Bind() const;
	void UnBind() const;

};