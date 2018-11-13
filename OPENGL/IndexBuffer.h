#pragma once
class IndexBuffer
{
	unsigned int Index_buffer_ID;
	unsigned int m_count;
public:
	IndexBuffer(const unsigned int* data, unsigned int size);
	~IndexBuffer();
	void Bind() const;
	void UnBind() const;

	inline unsigned int GetCount() const { return m_count; }

};