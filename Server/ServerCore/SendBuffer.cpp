#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(uint32 allocSize)
	: _allocSize(allocSize)
{
	_buffer.reserve(_allocSize);
}

SendBuffer::~SendBuffer()
{

}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
}